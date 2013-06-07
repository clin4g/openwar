/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#ifndef ButtonsView_H
#define ButtonsView_H

#include "bounds.h"
#include "texture.h"

#include "View.h"
#include "SimulationState.h"
#include "ButtonRendering.h"


class ButtonArea;
class ButtonView;


struct ButtonAlignment
{
	enum class Vertical { Top, Center, Bottom };
	enum class Horizontal { Left, Center, Right };

	static const ButtonAlignment TopLeft;
	static const ButtonAlignment TopCenter;
	static const ButtonAlignment TopRight;
	static const ButtonAlignment CenterLeft;
	static const ButtonAlignment Center;
	static const ButtonAlignment CenterRight;
	static const ButtonAlignment BottomLeft;
	static const ButtonAlignment BottomCenter;
	static const ButtonAlignment BottomRight;

	Vertical vertical;
	Horizontal horizontal;

	ButtonAlignment() : vertical(Vertical::Center), horizontal(Horizontal::Center) { }
	ButtonAlignment(Vertical v, Horizontal h) : vertical(v), horizontal(h) { }

	bool operator==(const ButtonAlignment& other) const { return vertical == other.vertical && horizontal == other.horizontal; }
	bool operator!=(const ButtonAlignment& other) const { return vertical != other.vertical || horizontal != other.horizontal; }
};


struct ButtonIcon
{
	texture* _texture;
	bounds2f bounds;
	glm::vec2 size;
	float scale;

	ButtonIcon() : _texture(nullptr), scale(1) { }
	ButtonIcon(texture* t, glm::vec2 s, bounds2f b) : _texture(t), bounds(b), size(s), scale(1) { }
};



class ButtonItem
{
	ButtonArea* _buttonArea;
	NSString* _buttonText;
	ButtonIcon* _buttonIcon;
	bool _hasAction;
	std::function<void()> _action;
	char _keyboardShortcut;
	bounds2f _bounds;
	bool _highlight;
	bool _selected;
	bool _disabled;

public:
	ButtonItem(ButtonArea* buttonArea, NSString* text);
	ButtonItem(ButtonArea* buttonArea, ButtonIcon* icon);
	~ButtonItem();

	ButtonArea* GetButtonArea() const { return _buttonArea; }

	NSString* GetButtonText() const { return _buttonText; }

	ButtonIcon* GetButtonIcon() const { return _buttonIcon; }
	void SetButtonIcon(ButtonIcon* value) { _buttonIcon = value; }

	ButtonItem* SetAction(std::function<void()> action) { _action = action; _hasAction = true; return this; }
	bool HasAction() const { return _hasAction; }
	void CallAction() const { _action(); }

	char GetKeyboardShortcut() const { return _keyboardShortcut; }
	void SetKeyboardShortcut(char value) { _keyboardShortcut = value; }

	bounds2f GetBounds() const { return _bounds; }
	void SetBounds(bounds2f value) { _bounds = value; }

	bool IsHighlight() const { return _highlight; }
	void SetHighlight(bool value) { _highlight = value; }

	bool IsSelected() const { return _selected; }
	void SetSelected(bool value) { _selected = value; }

	bool IsDisabled() const { return _disabled; }
	void SetDisabled(bool value) { _disabled = value; }

	glm::vec2 CalculateSize() const;
};


class ButtonArea
{
	ButtonView* _buttonView;

public:
	std::vector<ButtonItem*> buttonItems;
	std::vector<float> columns;
	std::vector<float> rows;
	bounds2f _bounds;
	std::function<void()> noaction;

	ButtonArea(ButtonView* buttonView, int numberOfColumns);
	~ButtonArea();

	ButtonView* GetButtonView() const { return _buttonView; }

	ButtonItem* AddButtonItem(NSString* buttonText);
	ButtonItem* AddButtonItem(ButtonIcon* buttonIcon);

	glm::vec2 CalculateSize() const;

	void UpdateColumnsAndRows();
	void UpdateBounds(bounds2f bounds);
};


class ButtonView : public View
{
public:
	ButtonRendering* _buttonRendering;
private:
	ButtonAlignment _alignment;
	std::vector<ButtonArea*> _buttonAreas;
	std::vector<ButtonArea*> _obsolete;

public:
	ButtonView(Surface* screen, ButtonRendering* buttonRendering, ButtonAlignment alignment);

	virtual void SetViewport(bounds2f value);

	const std::vector<ButtonArea*>& GetButtonAreas() const {  return _buttonAreas; }

	bool HasButtons() const;
	void Reset();

	ButtonArea* AddButtonArea(int numberOfColumns = 1);

	void UpdateLayout();

	virtual void Render();

	virtual void Update(double secondsSinceLastUpdate);
};


#endif
