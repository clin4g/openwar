// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef QUADTREE_H
#define QUADTREE_H


const int QuadTreeNodeItems = 16;


template <class T> class quadtree
{
	struct item
	{
		float _x, _y;
		T _value;
		item() : _x(), _y(), _value() {}
		item(float x, float y, T value) : _x(x), _y(y), _value(value) {}
	};

	struct node
	{
		node* _parent;
		node* _children[4];
		float _minX, _minY;
		float _maxX, _maxY;
		float _midX, _midY;
		int _minX100, _maxX100, _minY100, _maxY100;
		item _items[QuadTreeNodeItems];
		int _count;

		node(node* parent, float minX, float minY, float maxX, float maxY);
		~node();

		int get_index();
		int get_child_index(float x, float y);
		void split();
		void reset();
	};

	node _root;

public:
	class iterator
	{
		float _x, _y;
		int _x100, _y100;
		int _radius100;
		float _radiusSquared;
		node* _node;
		int _index;

	public:
		iterator(node* root, float x, float y, float radius);
		iterator(const iterator& i) :
		_x(i._x), _y(i._y),
		_x100(i._x100), _y100(i._y100),
		_radius100(i._radius100),
		_radiusSquared(i._radiusSquared),
		_node(i._node),
		_index(i._index) {}

		~iterator() {}

		T* operator*();

		iterator& operator++()
		{
			move_next();
			return *this;
		}

	private:
		bool is_within_radius(item* item);
		bool is_within_radius(node* node);
		bool is_within_radius(float x, float y);

		void move_next();
		node* get_next_node();
	};

public:
	quadtree(float minX, float minY, float maxX, float maxY);
	~quadtree();

	void insert(float x, float y, T value);
	void clear();

	iterator find(float x, float y, float radius);

private:
	static int convert(float value) { return (int)(value * 100); }
};




template <class T> quadtree<T>::quadtree(float minX, float minY, float maxX, float maxY) :
_root(0, minX, minY, maxX, maxY)
{
}



template <class T> quadtree<T>::~quadtree()
{
}



template <class T> void quadtree<T>::insert(float x, float y, T value)
{
	node* node = &_root;
	int level = 0;

	while (node->_children[0])
	{
		node = node->_children[node->get_child_index(x, y)];
		if (++level > 12)
			break;
	}

	while (node->_count == QuadTreeNodeItems)
	{
		node->split();
		if (++level > 12)
			break;
		node = node->_children[node->get_child_index(x, y)];
	}

	node->_items[node->_count++] = item(x, y, value);
}



template <class T> void quadtree<T>::clear()
{
    _root.reset();
}



template <class T> typename quadtree<T>::iterator quadtree<T>::find(float x, float y, float radius)
{
	return iterator(&_root, x, y, radius);
}



template <class T> quadtree<T>::node::node(node* parent, float minX, float minY, float maxX, float maxY)
: _parent(parent),
_minX(minX), _minY(minY),
_maxX(maxX), _maxY(maxY),
_midX((minX + maxX) / 2), _midY((minY + maxY) / 2),
_minX100(convert(minX)), _maxX100(convert(maxX)), _minY100(convert(minY)), _maxY100(convert(maxY)),
_count(0)
{
	_children[0] = 0;
	_children[1] = 0;
	_children[2] = 0;
	_children[3] = 0;
}



template <class T> quadtree<T>::node::~node()
{
	delete _children[0];
	delete _children[1];
	delete _children[2];
	delete _children[3];
}



template <class T> int quadtree<T>::node::get_index()
{
	if (this == _parent->_children[0]) return 0;
	if (this == _parent->_children[1]) return 1;
	if (this == _parent->_children[2]) return 2;
	if (this == _parent->_children[3]) return 3;
	return -1;
}



template <class T> int quadtree<T>::node::get_child_index(float x, float y)
{
	return (x > _midX ? 1 : 0) + (y > _midY ? 2 : 0);
}



template <class T> void quadtree<T>::node::split()
{
	if (!_children[0])
	{
		_children[0] = new node(this, _minX, _minY, _midX, _midY);
		_children[1] = new node(this, _midX, _minY, _maxX, _midY);
		_children[2] = new node(this, _minX, _midY, _midX, _maxY);
		_children[3] = new node(this, _midX, _midY, _maxX, _maxY);
	}

	for (int i = 0; i < _count; ++i)
	{
		item& item = _items[i];
		int index = get_child_index(item._x, item._y);
		node* child = _children[index];

		if (child->_count == QuadTreeNodeItems)
			child->split();

		child->_items[child->_count++] = item;
	}

	_count = 0;
}



template <class T> void quadtree<T>::node::reset()
{
	_count = 0;

	if (_children[0])
	{
		for (int i = 0; i < 4; ++i)
			_children[i]->reset();
	}
}



template <class T> quadtree<T>::iterator::iterator(node* root, float x, float y, float radius)
: _x(x), _y(y),
_x100(convert(x)), _y100(convert(y)),
_radius100(convert(radius)),
_radiusSquared(radius * radius),
_node(root),
_index(0)
{
	if (_node)
	{
		--_index;
		move_next();
	}
}



template <class T> T* quadtree<T>::iterator::operator*()
{
	return _node ? &_node->_items[_index]._value : 0;
}



template <class T> bool quadtree<T>::iterator::is_within_radius(item* item)
{
	return is_within_radius(item->_x, item->_y);
}



template <class T> bool quadtree<T>::iterator::is_within_radius(node* node)
{
	int minX = node->_minX100 - _radius100;
	if (_x100 < minX)
		return false;

    int maxX = node->_maxX100 + _radius100;
	if (_x100 > maxX)
		return false;

    int minY = node->_minY100 - _radius100;
	if (_y100 < minY)
		return false;

    int maxY = node->_maxY100 + _radius100;
    if (_y100 > maxY)
		return false;

	return true;
}



template <class T> bool quadtree<T>::iterator::is_within_radius(float x, float y)
{
	float dx = x - _x;
	float dy = y - _y;
	float distanceSquared = dx * dx + dy * dy;
	return distanceSquared <= _radiusSquared;
}



template <class T> void quadtree<T>::iterator::move_next()
{
	while (_node)
	{
		if (++_index == _node->_count)
		{
			_index = 0;
			_node = get_next_node();
			while (_node && !_node->_count)
				_node = get_next_node();
			if (!_node)
				return;
		}
		if (is_within_radius(&_node->_items[_index]))
			return;
	}
}



template <class T> typename quadtree<T>::node* quadtree<T>::iterator::get_next_node()
{
	if (_node->_children[0])
	{
		for (int index = 0; index < 4; ++index)
        {
            node* child = _node->_children[index];
			if (is_within_radius(child))
				return child;
        }
	}

	node* current = _node;
	while (current->_parent != nullptr)
	{
		int index = current->get_index();
		while (++index != 4)
        {
            node* sibling = current->_parent->_children[index];
			if (is_within_radius(sibling))
				return sibling;
		}

		current = current->_parent;
	}

	return 0;
}


#endif
