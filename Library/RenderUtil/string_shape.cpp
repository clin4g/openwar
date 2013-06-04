/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "string_shape.h"
#include "image.h"



image* string_font::_image = nullptr;



string_font::string_font(NSString* name, CGFloat size, float pixelDensity) :
_font(nil),
_pixelDensity(pixelDensity),
_items(),
_next(CGPointZero),
_dirty(false)
{
	initialize();

	size *= _pixelDensity;

#if TARGET_OS_IPHONE
	_font = [[UIFont fontWithName:name size:size] retain];
#else
	_font = [[NSFont fontWithName:name size:size] retain];
#endif
}



string_font::string_font(bool bold, CGFloat size, float pixelDensity) :
_font(nil),
_pixelDensity(pixelDensity),
_items(),
_next(CGPointZero),
_dirty(false)
{
	initialize();

	size *= _pixelDensity;

#if TARGET_OS_IPHONE
	if (bold)
		_font = [[UIFont boldSystemFontOfSize:size] retain];
	else
		_font = [[UIFont systemFontOfSize:size] retain];
#else
	if (bold)
		_font = [[NSFont boldSystemFontOfSize:size] retain];
	else
		_font = [[NSFont systemFontOfSize:size] retain];
#endif
}



string_font::~string_font()
{
	[_font release];
}



void string_font::initialize()
{
	if (_image == nullptr)
		_image = new image(512, 512, GL_RGBA);

	_renderer = new renderer<texture_alpha_vertex, string_uniforms>((
		VERTEX_ATTRIBUTE(texture_alpha_vertex, _position),
		VERTEX_ATTRIBUTE(texture_alpha_vertex, _texcoord),
		VERTEX_ATTRIBUTE(texture_alpha_vertex, _alpha),
		SHADER_UNIFORM(string_uniforms, _transform),
		SHADER_UNIFORM(string_uniforms, _texture),
		SHADER_UNIFORM(string_uniforms, _color),
		VERTEX_SHADER
		({
			attribute vec2 position;
			attribute vec2 texcoord;
			attribute float alpha;
			uniform mat4 transform;
			varying vec2 _texcoord;
			varying float _alpha;

			void main()
			{
				vec4 p = transform * vec4(position.x, position.y, 0, 1);

				_texcoord = texcoord;
				_alpha = alpha;

				gl_Position = vec4(p.x, p.y, 0.5, 1.0);
			}
		}),
		FRAGMENT_SHADER
		({
			uniform sampler2D texture;
			uniform vec4 color;
			varying vec2 _texcoord;
			varying float _alpha;

			void main()
			{
				vec4 result;
				result.rgb = color.rgb;
				result.a = texture2D(texture, _texcoord).a * color.a * clamp(_alpha, 0.0, 1.0);

				gl_FragColor = result;
			}
		})
	));
	_renderer->_blend_sfactor = GL_SRC_ALPHA;
	_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}



float string_font::font_size() const
{
	return (float)_font.pointSize / _pixelDensity;
}



float string_font::shadow_offset() const
{
	return 1 / _pixelDensity;
}




void string_font::add_character(unichar character)
{
	if (_items.find(character) != _items.end())
		return;

	NSString* text = [NSString stringWithCharacters:&character length:1];

#if TARGET_OS_IPHONE
	CGSize size = [text sizeWithFont:_font];
#else
	NSDictionary* attributes = [NSDictionary dictionaryWithObjectsAndKeys:_font, NSFontAttributeName, nil];
	CGSize size = [text sizeWithAttributes:attributes];
#endif

	if (_next.x + size.width > _image->_width)
	{
		_next.x = 0;
		_next.y += size.height + 1;
		if (_next.y + size.height > _image->_height)
			_next.y = 0; // TODO: handle texture resizing
	}

	item item;
	item._character = character;
	item._bounds.origin = _next;
	item._bounds.size = size;
	item._u0 = (float)item._bounds.origin.x / _image->_width;
	item._u1 = (float)(item._bounds.origin.x + item._bounds.size.width) / _image->_width;
	item._v0 = 1 - (float)(item._bounds.origin.y + item._bounds.size.height) / _image->_height;
	item._v1 = 1 - (float)item._bounds.origin.y / _image->_height;

	_items[character] = item;

	_next.x += floorf((float)item._bounds.size.width + 1) + 1;
	_dirty = true;
}



string_font::item string_font::get_character(unichar character) const
{
	auto i = _items.find(character);
	if (i != _items.end())
		return i->second;

	return _items.find('.')->second;
}



void string_font::update_texture()
{
	if (!_dirty)
		return;

#if TARGET_OS_IPHONE
	UIGraphicsPushContext(_image->_context);
#else
	NSGraphicsContext *gc = [NSGraphicsContext graphicsContextWithGraphicsPort:_image->_context flipped:YES];
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:gc];
#endif

	CGContextClearRect(_image->_context, CGRectMake(0, 0, _image->_width, _image->_height));

	for (std::map<unichar, item>::iterator i = _items.begin(); i != _items.end(); ++i)
	{
		item item = (*i).second;

		NSString *text = [NSString stringWithCharacters:&item._character length:1];

		CGContextSetRGBFillColor(_image->_context, 1, 1, 1, 1);

#if TARGET_OS_IPHONE
	    [text drawAtPoint:item._bounds.origin withFont:_font];
#else
		NSDictionary* attributes = [NSDictionary dictionaryWithObjectsAndKeys:_font, NSFontAttributeName, nil];
		[text drawAtPoint:item._bounds.origin withAttributes:attributes];
#endif
	}

	_texture.load(*_image);

#if TARGET_OS_IPHONE
	UIGraphicsPopContext();
#else
	[NSGraphicsContext restoreGraphicsState];
#endif

	_dirty = false;
}



glm::vec2 string_font::measure(NSString* string)
{
	//return vector2([string sizeWithFont:_font]) * size / _font.pointSize;

	for (NSUInteger i = 0; i < string.length; ++i)
	{
		unichar character = [string characterAtIndex:i];
		add_character(character);
	}

	float w = 0;
	float h = 0;

	for (NSUInteger i = 0; i < string.length; ++i)
	{
		unichar character = [string characterAtIndex:i];
		item item = get_character(character);
		glm::vec2 s = get_size(item);
		w += s.x;
		h = fmaxf(h, s.y);
	}

	return glm::vec2(w, h);
}



glm::vec2 string_font::get_size(const item& item) const
{
	return glm::vec2(item._bounds.size.width, item._bounds.size.height) / _pixelDensity;

	/*float h = size;
	float w = item._bounds.size.width * size / item._bounds.size.height;
	return vector2(w, h) / pixel_scale();*/
}



/***/



string_shape::string_shape(string_font* font) : _font(font)
{
	_mode = GL_TRIANGLES;
}



void string_shape::clear()
{
	_vertices.clear();
}

#if !defined(ENABLE_BIDIRECTIONAL_TEXT)
#error ENABLE_BIDIRECTIONAL_TEXT not defined
#endif

#if ENABLE_BIDIRECTIONAL_TEXT


#include "unicode/ubidi.h"


static unichar* ReserveSrcBuffer(int required)
{
	static unichar* buffer = nullptr;
	static int reserved = 0;

	if (buffer == nullptr || required > reserved)
	{
		delete buffer;
		buffer = new unichar[required];
		reserved = required;
	}

	return buffer;
}


static unichar* ReserveDstBuffer(int required)
{
	static unichar* buffer = nullptr;
	static int reserved = 0;

	if (buffer == nullptr || required > reserved)
	{
		delete buffer;
		buffer = new unichar[required];
		reserved = required;
	}

	return buffer;
}



static bool MayNeedReorder(unichar c)
{
	return c > 255;
}


static bool CanSkipReorder(NSString* string)
{
	NSUInteger length = string.length;
	if (length > 16)
		return false;

	static unichar buffer[16];
	[string getCharacters:buffer];

	for (NSUInteger i = 0; i < length; ++i)
		if (MayNeedReorder(buffer[i]))
			return false;

	return true;
}



static NSString* ReorderToDisplayDirection(NSString* string)
{
	if (CanSkipReorder(string))
		return string;

	UErrorCode error = U_ZERO_ERROR;
	int length = (int)string.length;

	unichar* src = ReserveSrcBuffer(length);
	unichar* dst = ReserveDstBuffer(length * 2);

	UBiDi* ubidi = ubidi_openSized(length, 0, &error);
    if (error != 0)
        NSLog(@"%04x", error);

	[string getCharacters:src];
    ubidi_setPara(ubidi, src, length, UBIDI_DEFAULT_LTR, NULL, &error);
    if (error != 0)
        NSLog(@"%04x", error);

	length = ubidi_writeReordered(ubidi, dst, length * 2, UBIDI_DO_MIRRORING | UBIDI_REMOVE_BIDI_CONTROLS, &error);
    if (error != 0)
        NSLog(@"%04x", error);
    
    NSString* result = [NSString stringWithCharacters:dst length:(NSUInteger)length];

    ubidi_close(ubidi);

	return result;
}

#endif



void string_shape::add(NSString* string, glm::mat4x4 transform, float alpha, float delta)
{
	#if ENABLE_BIDIRECTIONAL_TEXT
    string = ReorderToDisplayDirection(string);
    #endif

	for (NSUInteger i = 0; i < string.length; ++i)
	{
		unichar character = [string characterAtIndex:i];
		_font->add_character(character);
	}

	glm::vec2 p(0, 0);

	for (NSUInteger i = 0; i < string.length; ++i)
	{
		unichar character = [string characterAtIndex:i];
		string_font::item item = _font->get_character(character);

		glm::vec2 s = _font->get_size(item);
		bounds2f bounds = bounds2_from_corner(p, s);
		bounds.min = (transform * glm::vec4(bounds.min.x, bounds.min.y, 0, 1)).xy();
		bounds.max = (transform * glm::vec4(bounds.max.x, bounds.max.y, 0, 1)).xy();

		float next_alpha = alpha + delta * s.x;

		_vertices.push_back(texture_alpha_vertex(bounds.p11(), glm::vec2(item._u0, item._v0), alpha));
		_vertices.push_back(texture_alpha_vertex(bounds.p12(), glm::vec2(item._u0, item._v1), alpha));
		_vertices.push_back(texture_alpha_vertex(bounds.p22(), glm::vec2(item._u1, item._v1), next_alpha));

		_vertices.push_back(texture_alpha_vertex(bounds.p22(), glm::vec2(item._u1, item._v1), next_alpha));
		_vertices.push_back(texture_alpha_vertex(bounds.p21(), glm::vec2(item._u1, item._v0), next_alpha));
		_vertices.push_back(texture_alpha_vertex(bounds.p11(), glm::vec2(item._u0, item._v0), alpha));

		if (next_alpha < 0)
			break;

		p.x += s.x;
		alpha = next_alpha;
	}
}


void string_shape::update(GLenum usage)
{
	_font->update_texture();
	shape<texture_alpha_vertex>::update(usage);
}
