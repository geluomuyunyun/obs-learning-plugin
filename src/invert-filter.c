#include <obs-module.h>


struct invert_filter {
	obs_source_t *context; 
	gs_effect_t *effect;   
};

static const char *invert_get_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("InvertFilter");
}

static void *invert_create(obs_data_t *settings, obs_source_t *source)
{
	UNUSED_PARAMETER(settings);

	struct invert_filter *filter = bzalloc(sizeof(struct invert_filter));
	filter->context = source;
	char *path = obs_module_file("effects/invert.effect");

	obs_enter_graphics();
	filter->effect = gs_effect_create_from_file(path, NULL);
	obs_leave_graphics();

	bfree(path);

	if (!filter->effect) {
		bfree(filter);
		return NULL;
	}
	return filter;
}

static void invert_destroy(void *data)
{
	struct invert_filter *filter = data;

	obs_enter_graphics();
	gs_effect_destroy(filter->effect);
	obs_leave_graphics();

	bfree(filter);
}

static void invert_render(void *data, gs_effect_t *effect)
{
	UNUSED_PARAMETER(effect);
	struct invert_filter *filter = data;

	if (!obs_source_process_filter_begin(filter->context, GS_RGBA, OBS_ALLOW_DIRECT_RENDERING))
		return;

	obs_source_process_filter_end(filter->context, filter->effect, 0, 0);
}


struct obs_source_info invert_filter_info = {
	.id = "my_invert_filter", 	.type = OBS_SOURCE_TYPE_FILTER,
	.output_flags = OBS_SOURCE_VIDEO,
	.get_name = invert_get_name,
	.create = invert_create,
	.destroy = invert_destroy,
	.video_render = invert_render,
};