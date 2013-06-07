/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "vertexbuffer.h"
#include "renderer.h"

#ifndef TERRAIN_H
#define TERRAIN_H

#include "heightmap.h"
#include "framebuffer.h"
#include "renderbuffer.h"


struct terrain_renderers;

struct terrain_address
{
	int _level; // level 0 has an unsplit chunk (1x1), level 1 is split 2x2, level 2 is split (2x2)x(2x2) and so on.
	int _x; // 0 <= _x <= 2 ^ _level
	int _y; // 0 <= _x <= 2 ^ _level

	terrain_address();
	terrain_address(int level, int x, int y);

	static bool is_valid(int level, int x, int z);
	terrain_address get_parent();
	void foreach_neighbor(std::function<void (terrain_address)> action);
	void foreach_child(std::function<void (terrain_address)> action);
	void foreach_ancestor(std::function<void (terrain_address)> action);

	bool all_children(std::function<bool (terrain_address)> predicate);
	bool all_ancestors(std::function<bool (terrain_address)> predicate);
};

bool operator ==(terrain_address chunk1, terrain_address chunk2);
bool operator !=(terrain_address chunk1, terrain_address chunk2);
bool operator <(terrain_address chunk1, terrain_address chunk2);



struct terrain_vertex
{
	glm::vec3 _position;
	glm::vec3 _normal;

	terrain_vertex(glm::vec3 p, glm::vec3 n) : _position(p), _normal(n) {}
};


struct terrain_edge_vertex
{
	glm::vec3 _position;
	float _height;

	terrain_edge_vertex() {}
	terrain_edge_vertex(glm::vec3 p, float h) : _position(p), _height(h) { }
};


struct terrain_uniforms
{
	glm::mat4x4 _transform;
	glm::vec3 _light_normal;
	const texture* _colors;
	const texture* _forest;
};


class terrain_chunk
{
public:
	terrain_address _address;
	terrain_chunk* _parent;
	terrain_chunk* _neighbors[4];
	terrain_chunk* _children[4];
	bool _is_split;
	int _lod;
	shape<color_vertex3> _lines;
	shape<terrain_vertex> _inside;
	shape<terrain_vertex> _border;
	bounds3f _bounds;

	terrain_chunk(terrain_address address);
	bool has_children() const;

	shape<terrain_vertex>* triangle_shape(int inside);
};


struct sobel_uniforms
{
	glm::mat4x4 _transform;
	const texture* _depth;
};



struct terrain
{
	int _framebuffer_width;
	int _framebuffer_height;
	framebuffer* _framebuffer;
	texture* _depth;
	texture* _colors;
	texture* _forest;

	heightmap* _heightmap;
	std::map<terrain_address, terrain_chunk*> _chunks;
	std::map<terrain_address, bool> _split;
	std::map<terrain_address, float> _lod;

	shape<terrain_edge_vertex> _shape_terrain_edge;
	terrain_renderers* _renderers;

	terrain(heightmap* height, image* forest, bool render_edges);
	~terrain();

	void update_heights(bounds2f bounds);

	void update_depth_texture_size();
	void initialize_edge();

	void render(const terrain_uniforms& uniforms);
	void foreach_leaf(terrain_address chunk, std::function<void(terrain_chunk&)> f);

	bool is_loaded(terrain_address chunk);
	void load_chunk(terrain_address chunk, float priority);
	void unload_chunk(terrain_address chunk);

	void load_children(terrain_address chunk, float priority);
	void request_load_children_unload_grand_children(terrain_address chunk, float priority);
	void request_unload_children(terrain_address chunk);

	terrain_chunk* create_node(terrain_address chunk);

	bool is_split(terrain_address chunk);
	void set_split(terrain_address chunk);
	void clear_split(terrain_address chunk);
	bool can_chunk_be_splitted(terrain_address chunk);
	bool can_grand_parent_be_splitted(terrain_address chunk);

	void set_lod(terrain_address chunk, float lod);
	float get_lod(terrain_address chunk);

	bounds3f get_bounds(terrain_address chunk) const;

	void build_lines(shape<color_vertex3>& shape, terrain_address chunk);
	void build_triangles(terrain_chunk* chunk);

	terrain_vertex make_terrain_vertex(float x, float y);
	color_vertex3 make_color_vertex(float x, float y);
};


struct terrain_viewpoint
{
	terrain* _terrain;
	glm::vec3 _viewpoint;
	float _near;
	float _far;
	int _near_lod;
	float _distance_lod_max;

	terrain_viewpoint(terrain* terrain);

	void set_parameters(float errorLodMax, float maxPixelError, float screenWidth, float horizontalFOVDegrees);

	float compute_lod(bounds3f boundingBox) const;
	float compute_lod(float distance) const;

	void update();
	void update(terrain_address chunk);


};


#endif
