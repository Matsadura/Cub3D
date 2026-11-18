#include "render.h"

#include <math.h>
#include <stdlib.h>

/*
** Lighting model: a little ambient light that dies off quickly, plus the
** flashlight beam (a soft disc in the middle of the screen with a dimmer
** spill around it) falling off with distance. The beam shape and the
** vignette only depend on the pixel, so they are precomputed once.
*/

#define AMBIENT		0.10f
#define AMB_FALL	0.55f
#define BEAM_RANGE	5.0
#define GLOW		0xFE

static float	smooth(float e0, float e1, float x)
{
	float	t;

	t = (x - e0) / (e1 - e0);
	if (t < 0.0f)
		t = 0.0f;
	if (t > 1.0f)
		t = 1.0f;
	return (t * t * (3.0f - 2.0f * t));
}

bool	light_init(t_renderer *r)
{
	int		x;
	int		y;
	float	cx;
	float	cy;
	float	d;

	r->cone = malloc(sizeof(float) * VIEW_W * VIEW_H);
	r->vignette = malloc(sizeof(float) * VIEW_W * VIEW_H);
	if (r->cone == NULL || r->vignette == NULL)
		return (false);
	y = -1;
	while (++y < VIEW_H)
	{
		x = -1;
		while (++x < VIEW_W)
		{
			cx = (x - VIEW_W / 2.0f) / (VIEW_W / 2.0f);
			cy = (y - VIEW_H / 2.0f) / (VIEW_W / 2.0f);
			d = sqrtf(cx * cx + cy * cy);
			r->cone[y * VIEW_W + x] = 1.0f - smooth(0.18f, 0.55f, d) * 0.72f
				- smooth(0.5f, 1.1f, d) * 0.23f;
			cy = (y - VIEW_H / 2.0f) / (VIEW_H / 2.0f);
			r->vignette[y * VIEW_W + x] = smooth(0.55f, 1.45f,
					sqrtf(cx * cx + cy * cy));
		}
	}
	return (true);
}

float	light_at(const t_renderer *r, int i, double d)
{
	float	amb;
	float	beam;

	amb = AMBIENT / (1.0f + AMB_FALL * (float)(d * d));
	beam = 0.0f;
	if (r->beam > 0.0)
		beam = (float)r->beam * r->cone[i]
			* 1.1f / (1.0f + (float)((d / BEAM_RANGE) * (d / BEAM_RANGE)) * 2.2f);
	return (amb + beam);
}

bool	is_emissive(uint32_t argb)
{
	return ((argb >> 24) == GLOW);
}

uint32_t	shade(uint32_t argb, float k)
{
	int	f;
	int	rr;
	int	gg;
	int	bb;

	if (k >= 1.0f)
		k = 1.0f + (k - 1.0f) * 0.4f;
	f = (int)(k * 256.0f);
	rr = (int)((argb >> 16) & 0xFF) * f >> 8;
	gg = (int)((argb >> 8) & 0xFF) * f >> 8;
	bb = (int)(argb & 0xFF) * f >> 8;
	if (rr > 255)
		rr = 255;
	if (gg > 255)
		gg = 255;
	if (bb > 255)
		bb = 255;
	return (0xFF000000u | (uint32_t)rr << 16 | (uint32_t)gg << 8
		| (uint32_t)bb);
}
