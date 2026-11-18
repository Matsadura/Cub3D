#include "game.h"
#include "platform.h"
#include "render.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __EMSCRIPTEN__
# include <emscripten.h>
#endif

#define MAX_DT			0.1
#define NATIVE_FPS		240
#define SCRIPT_MOUSE_PX	100.0f
#define WINDOW_TITLE	"cub3D"

static const char *const	g_default_levels[] = {
	"assets/maps/subject_map.cub",
	"assets/maps/dungeon.cub",
	"assets/maps/library.cub",
	"assets/maps/cheese_maze.cub",
	"assets/maps/creepy.cub",
};

typedef struct s_app
{
	t_platform	*platform;
	t_game		game;
	t_renderer	renderer;
	char		**custom_levels;
	int			custom_count;
	double		last_time;
	bool		running;
	int			exit_code;
	long		frame_limit;
	long		frames;
	const char	*script;
	const char	*screenshot;
	double		fixed_dt;
}	t_app;

static void	usage(const char *prog)
{
	fprintf(stderr,
		"Usage: %s [options] [map.cub ...]\n"
		"  With no maps, plays the built-in maps (assets/maps).\n"
		"  --check          validate the given maps and exit\n"
		"  --frames N       quit after N frames (testing)\n"
		"  --dt SECONDS     fixed simulation step per frame (testing)\n"
		"  --screenshot F   save the last --frames frame as a BMP\n"
		"  --keys STR       scripted input, one char per frame:\n"
		"                   w/s forward/back, a/d strafe, j/l turn,\n"
		"                   < > mouse look, e enter, r restart, p pause,\n"
		"                   n minimap, m mute, q quit, . wait\n",
		prog);
}

static void	print_map_error(const char *path, const t_map *m, t_map_error e)
{
	char	buf[256];

	map_describe_error(m, e, buf, sizeof(buf));
	printf("%s: Error: %s\n", path, buf);
}

static int	check_maps(char **paths, int count)
{
	t_map		map;
	t_map_error	err;
	int			failed;
	int			i;

	failed = 0;
	i = 0;
	while (i < count)
	{
		err = map_load(paths[i], &map);
		if (err == MAP_OK)
			printf("%s: OK (%dx%d)\n", paths[i], map.w, map.h);
		else
			print_map_error(paths[i], &map, err);
		failed += (err != MAP_OK);
		map_free(&map);
		i++;
	}
	return (failed != 0);
}

static void	app_shutdown(t_app *app)
{
	int	i;

	renderer_free(&app->renderer);
	game_free(&app->game);
	platform_destroy(app->platform);
	app->platform = NULL;
	i = 0;
	while (i < app->custom_count)
		free(app->custom_levels[i++]);
	free(app->custom_levels);
	app->custom_levels = NULL;
	app->custom_count = 0;
}

/* One char per frame; movement chars hold that action for the frame. */
static void	scripted_input(t_app *app, t_input *in)
{
	static const char		held[] = "wsadjl";
	static const char		press[] = "erpnm";
	static const t_action	press_act[] = {ACT_CONFIRM, ACT_RESTART,
		ACT_PAUSE, ACT_MAP, ACT_MUTE};
	const char				*k;
	char					c;

	if (app->script == NULL || *app->script == '\0')
		return ;
	c = *app->script++;
	k = strchr(held, c);
	if (k != NULL)
		in->held |= ACT_BIT((t_action)(ACT_FORWARD + (k - held)));
	k = strchr(press, c);
	if (k != NULL)
		input_push(in, press_act[k - press]);
	if (c == '<' || c == '>')
		in->mouse_dx += (c == '<') ? -SCRIPT_MOUSE_PX : SCRIPT_MOUSE_PX;
	if (c == 'q')
		in->quit_requested = true;
}

#ifndef __EMSCRIPTEN__

static void	print_status(const t_game *g)
{
	static const char	*names[GAME_STATE_COUNT] = {"PLAYING", "PAUSED",
		"FATAL"};

	printf("[status] level=%d state=%s x=%.2f y=%.2f angle=%.1f minimap=%d "
		"time=%.2f\n", g->level_index + 1, names[g->state], g->player.x,
		g->player.y, g->player.angle * 180.0 / PI_D, g->show_minimap,
		g->level_time);
}
#endif

static void	fail(t_app *app, const char *msg)
{
	fprintf(stderr, "Error: %s\n", msg);
	app->running = false;
	app->exit_code = EXIT_FAILURE;
}

/* Draws the frame; the last frame of a --frames run can be saved first. */
static void	present(t_app *app)
{
	renderer_draw(&app->renderer, app->platform, &app->game);
	app->frames++;
	if (app->screenshot != NULL && app->frames == app->frame_limit
		&& !platform_save_screenshot(app->platform, app->screenshot))
		fprintf(stderr, "Warning: screenshot failed: %s\n", platform_error());
	platform_present(app->platform);
}

/* One frame: input -> update(dt) -> render. Never blocks. */
static void	tick(void *arg)
{
	t_app	*app;
	t_input	in;
	double	now;
	double	dt;

	app = arg;
	now = platform_time_seconds();
	dt = now - app->last_time;
	app->last_time = now;
	if (dt > MAX_DT)
		dt = MAX_DT;
	if (app->fixed_dt > 0.0)
		dt = app->fixed_dt;
	app->game.events = 0;
	platform_poll_input(app->platform, &in);
	scripted_input(app, &in);
	if (in.quit_requested || input_pressed(&in, ACT_QUIT))
		app->running = false;
	else
	{
		game_update(&app->game, &in, dt);
		if (app->game.state == GAME_FATAL)
			fail(app, map_error_str(app->game.error));
		else if (!renderer_sync(&app->renderer, &app->game))
			fail(app, app->renderer.error);
		else
			present(app);
	}
	if (app->frame_limit > 0 && app->frames >= app->frame_limit)
		app->running = false;
#ifdef __EMSCRIPTEN__
	if (!app->running)
	{
		emscripten_cancel_main_loop();
		app_shutdown(app);
	}
#endif
}

/* Resolves user map paths before platform_locate_assets() may chdir. */
static bool	store_custom_levels(t_app *app, char **paths, int count)
{
	char	resolved[PATH_MAX];

	app->custom_levels = calloc(count, sizeof(char *));
	if (app->custom_levels == NULL)
		return (false);
	while (app->custom_count < count)
	{
		if (realpath(paths[app->custom_count], resolved) != NULL)
			app->custom_levels[app->custom_count] = strdup(resolved);
		else
			app->custom_levels[app->custom_count]
				= strdup(paths[app->custom_count]);
		if (app->custom_levels[app->custom_count] == NULL)
			return (false);
		app->custom_count++;
	}
	return (true);
}

static int	parse_args(t_app *app, int ac, char **av, bool *check)
{
	int	i;

	i = 1;
	while (i < ac && strncmp(av[i], "--", 2) == 0)
	{
		if (strcmp(av[i], "--check") == 0)
			*check = true;
		else if (strcmp(av[i], "--frames") == 0 && i + 1 < ac)
			app->frame_limit = strtol(av[++i], NULL, 10);
		else if (strcmp(av[i], "--dt") == 0 && i + 1 < ac)
			app->fixed_dt = strtod(av[++i], NULL);
		else if (strcmp(av[i], "--keys") == 0 && i + 1 < ac)
			app->script = av[++i];
		else if (strcmp(av[i], "--screenshot") == 0 && i + 1 < ac)
			app->screenshot = av[++i];
		else
			return (-1);
		i++;
	}
	return (i);
}

static int	start(t_app *app)
{
	t_map_error	err;
	char		msg[256];

	if (!platform_locate_assets())
		return (fprintf(stderr, "Error: assets/ directory not found\n"), 1);
	if (app->custom_count > 0)
		err = game_init(&app->game, (const char *const *)app->custom_levels,
				app->custom_count);
	else
		err = game_init(&app->game, g_default_levels,
				sizeof(g_default_levels) / sizeof(g_default_levels[0]));
	if (err != MAP_OK)
	{
		map_describe_error(&app->game.map, err, msg, sizeof(msg));
		fprintf(stderr, "Error: %s: %s\n", app->game.levels[0], msg);
		return (1);
	}
	app->platform = platform_create(WINDOW_TITLE, VIEW_W, VIEW_H);
	if (app->platform == NULL)
		return (fprintf(stderr, "Error: SDL: %s\n", platform_error()), 1);
	renderer_init(&app->renderer);
	if (!renderer_sync(&app->renderer, &app->game))
		return (fprintf(stderr, "Error: %s\n", app->renderer.error), 1);
	if (app->script == NULL && app->frame_limit == 0)
		platform_capture_mouse(app->platform, true);
	return (0);
}

int	main(int ac, char **av)
{
	static t_app	app;
	bool			check;
	int				first;
	double			frame_start;

	check = false;
	first = parse_args(&app, ac, av, &check);
	if (first < 0 || (check && first >= ac))
		return (usage(av[0]), EXIT_FAILURE);
	if (check)
		return (check_maps(av + first, ac - first));
	if (first < ac && !store_custom_levels(&app, av + first, ac - first))
		return (app_shutdown(&app), perror("cub3D"), EXIT_FAILURE);
	if (start(&app) != 0)
		return (app_shutdown(&app), EXIT_FAILURE);
	app.running = true;
	app.last_time = platform_time_seconds();
#ifdef __EMSCRIPTEN__
	(void)frame_start;
	emscripten_set_main_loop_arg(tick, &app, 0, 1);
#else
	while (app.running)
	{
		frame_start = platform_time_seconds();
		tick(&app);
		platform_limit_fps(app.platform, frame_start, NATIVE_FPS);
	}
	if (app.script != NULL || app.frame_limit > 0)
		print_status(&app.game);
	app_shutdown(&app);
#endif
	return (app.exit_code);
}
