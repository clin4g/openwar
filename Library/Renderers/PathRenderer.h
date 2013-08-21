#ifndef PathRenderer_H
#define PathRenderer_H

class GradientTriangleRenderer;


class PathRenderer
{
public:
	PathRenderer();
	~PathRenderer();

	static void Path(GradientTriangleRenderer* renderer, const std::vector<glm::vec2>& path, std::function<glm::vec3(glm::vec2)> getPosition, int mode);

private:
	static void RenderPath(GradientTriangleRenderer* renderer, const std::vector<glm::vec2>& path, std::function<glm::vec3(glm::vec2)> getPosition);
};



#endif
