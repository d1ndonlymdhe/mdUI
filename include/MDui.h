#pragma once
#include<iostream>
#include <raylib.h>
#include <functional>
#include <string>
#include <vector>

typedef struct V2 {
	int x;
	int y;
	V2(int x, int y) {
		this->x = x;
		this->y = y;
	}
};

class Text {
public:
	int fontSize, x, y;
	char* value;
	Color color;
	Text(char* value, int fontSize, Color color, int x, int y) :fontSize(fontSize), color(color), value(value), x(x), y(y) {

	}
	Text(char* value, int fontSize, Color color) :fontSize(fontSize), color(color), value(value), x(0), y(0) {

	}
	void draw(int scrollX, int scrollY) {
		DrawText(value, x + scrollX, y + scrollY, fontSize, color);
	}
	Text& updatePos(int x, int y) {
		this->x = x;
		this->y = y;
		return *this;
	}
	int getHeight() {
		return fontSize;
	}
	int getWidth() {
		return MeasureText((const char*)value, fontSize);
	}

};

class Input;

class Base {
protected:
	int x, y, height, width;
	Vector2 padding;
	Vector2 margin;
	Color bgColor;
	std::string value = "";
	std::vector<Base*> children;
	Text* innerText = NULL;
	int focus = -1;
	std::function<void(Base*)> onClick = [&](Base* self)->void {};
	std::function<void(Base*, char)> onKeyPress = [&](Base* self, char c)->void {};
	std::function<void(Base*)> onChange = [&](Base* self)->void {};
	bool fill = true;
public:
	Base() {
		x = 0;
		y = 0;
		height = 0;
		width = 0;
		padding = { 0,0 };
		margin = { 0,0 };
		bgColor = WHITE;
		value = "";
		children = {};
		innerText = NULL;
		focus = -1;
		onClick = [&](Base* self)->void {};
		onKeyPress = [&](Base* self, char c)->void {};
		fill = true;
	}
	Base(int height, int width, const Color& bgColor)
	{
		this->x = 0;
		this->y = 0;
		this->height = height;
		this->width = width;
		this->bgColor = bgColor;
		this->padding = Vector2{ 0,0 };
		this->margin = Vector2{ 0,0 };
	}
	Base(int height, int width, int x, int y, Vector2 padding, Vector2 margin, const Color& bgColor)
	{
		this->x = x;
		this->y = y;
		this->height = height;
		this->width = width;
		this->bgColor = bgColor;
		this->padding = padding;
		this->margin = margin;
	}
	Base& addText(Text* text) {
		text->x = x + padding.x;
		text->y = y + padding.y;
		innerText = text;
		return *this;
	}
	virtual Base& addChild(Base& child) {
		child.updatePos(child.x + x + padding.x, child.y + padding.y + y);
		if (child.fill) {
			child.updateDim(width - padding.x * 2, height - padding.y * 2);
		}
		children.push_back(&child);
		return *this;
	}
	virtual void draw(int scrollX, int scrollY) {
		DrawRectangle(x + margin.x + scrollX, y + margin.y + scrollY, width - margin.x, height - margin.y, bgColor);
		for (size_t i = 0; i < children.size(); i++) {
			Base* child = children[i];
			child->draw(scrollX, scrollY);

		}


		if (getInnerText()) {
			getInnerText()->draw(scrollX, scrollY);
		}
	}
	Base& updatePos(int x, int y) {
		this->x = x;
		this->y = y;
		for (size_t i = 0; i < children.size(); i++) {
			Base* child = children[i];
			//child->updateDim(child->x - margin.x, child->y - margin.y);
			child->updatePos(padding.x + x + margin.x, padding.y + y + margin.y);
		}

		if (getInnerText() != NULL) {
			getInnerText()->updatePos(padding.x + x + margin.x, padding.y + y + margin.y);
		}
		return *this;
	}
	Base& updateDim(int width, int height) {
		this->height = height;
		this->width = width;

		for (size_t i = 0; i < children.size(); i++) {
			Base* child = children[i];
			if (child->fill) {
				std::cout << width - padding.x * 2 << " " << height - padding.y * 2 << std::endl;
				child->updateDim(width - padding.x * 2, height - padding.y * 2);
			}
		}
		return *this;
	}
	Base& updateBg(Color color) {
		this->bgColor = color;
		return *this;
	}
	Base& updatePadding(Vector2 padding) {
		this->padding = padding;
		updatePos(this->x, this->y);
		return *this;
	}
	Base& updateMargin(Vector2 margin) {
		this->margin = margin;
		return *this;
	}
	Rectangle getComputedRect() {
		return Rectangle{
			x + margin.x,
			y + margin.y,
			width - margin.x,
			height - margin.y
		};
	}
	void updateFocus(V2 mousePos) {
		int mouseX = mousePos.x;
		int mouseY = mousePos.y;
		for (size_t i = 0; i < children.size(); i++) {
			Base* child = children[i];
			Rectangle childRect = child->getComputedRect();
			if (CheckCollisionPointRec({ (float)mouseX,(float)mouseY }, childRect)) {
				focus = i;
				child->updateFocus(mousePos);
				return;
			}
		}
		focus = -1;
	}
	Base* focusedEl() {
		if (focus == -1) {
			return this;
		}
		return children[focus]->focusedEl();
	}

	bool& getFill() {
		return fill;
	}

	std::string& getValue() {
		return value;
	}

	virtual std::function<void(Base*, char)> getKeyHandler() {
		return onKeyPress;
	}
	virtual void modifyText(const char* str) {
		innerText->value = (char*)str;
	}
	virtual Text* getInnerText() {
		return innerText;
	}
	virtual int getMaxHeight() {
		Rectangle selfRect = getComputedRect();

		int maxHeight = selfRect.y + selfRect.height;
		if (children.size() > 0) {
			for (size_t i = 0; i < children.size(); i++) {
				Base& child = *children[i];
				int childMaxHeight = child.getMaxHeight();
				if (childMaxHeight > maxHeight) {
					maxHeight = childMaxHeight;
				}
			}
		}
		return maxHeight;
	}
	//CHECK FOR NULL
	virtual std::function<void(Base*)>& getOnChange() {
		return onChange;
	};
	virtual void setFill(bool fill) {
		this->fill = fill;
	}
	virtual std::function<void(Base*)>& getOnClick() {
		return onClick;
	}
};

enum Align {
	LEFT,
	RIGHT,
	CENTER
};


class Grid :public Base {
public:
	size_t cols, rows;
	size_t nextRow = 0, nextCol = 0;
	int* templateCols, * templateRows;
	Grid(size_t rows, size_t cols, int* templateRows, int* templateCols, int x, int y, int height, int width, const Color& bgColor = WHITE) {
		this->cols = cols > 1 ? cols : 1;
		this->rows = rows > 1 ? rows : 1;
		this->height = height;
		this->width = width;
		this->templateCols = (int*)calloc(cols, sizeof(int));
		this->templateRows = (int*)calloc(rows, sizeof(int));
		this->templateCols = templateCols;
		this->templateRows = templateRows;
	}
	Grid& addChild(Base& child) {
		size_t childCount = children.size();
		if (children.size() < cols * rows) {

			size_t& xOff = nextCol;
			size_t& yOff = nextRow;

			if (child.getFill()) {
				child.updateDim(width * templateCols[nextCol] / 100, height * templateRows[nextRow] / 100);
			}

			bool colReset = (nextCol + 1) == cols;
			bool colInc = nextCol < cols;

			int newChildX = x;
			int newChildY = y;
			if (children.size()>0) {

				Rectangle prevChildRect = children[children.size()-1]->getComputedRect();
				if (colInc) {
					newChildX = prevChildRect.x + prevChildRect.width;
				}
				if (colReset) {
					newChildX = 0;
					newChildY = prevChildRect.y + prevChildRect.height;
				}
			}


			child.updatePos(newChildX, newChildY);
			children.push_back(&child);
			if (colInc) {
				nextCol++;
				if (colReset) {
					nextRow++;
					nextCol = 0;
				}
			}
		}
		return *this;
	}
	Grid& updateRowCols(size_t rows, size_t cols) {
		this->cols = cols;
		this->rows = rows;

		size_t nextCol = 0, nextRow = 0;
		for (size_t i = 0; i < children.size(); i++) {
			Base& child = *children[i];
			size_t& xOff = nextCol;
			size_t& yOff = nextRow;
			child.updateDim(height / rows, width / cols);
			if (child.getFill()) {
				child.updatePos(x + xOff * (width / cols), y + yOff * (height / rows));
			}
			children[i] = &child;
			if (nextCol < cols) {
				nextCol++;
				if (nextCol == cols) {
					nextRow++;
					nextCol = 0;
				}
			}

		}
		return *this;
	}
};

class Input : public Base {
protected:
	Color bgColor = WHITE;
	std::string value = "";
	Text* innerText = new Text((char*)value.c_str(), 0, BLACK);
	std::function<void(Base*)> onChange = [&](Base* self)->void {};
public:
	Input() {
		this->getValue() = "";
		this->innerText = new Text((char*)(std::string("")).c_str(), 20, BLACK);
	}
	std::function<void(void)> onClick = [&]()->void {

		};
	std::function<void(Base*, char)> onKeyPress = [&](Base* self, char c)->void {
		if (c == 8) {
			if (self->getValue().size() > 0) {
				self->getValue().pop_back();
			}
		}
		else {
			self->getValue() += c;
		}
		//self->innerText->value = (char*)self->value.c_str();
		self->modifyText(self->getValue().c_str());
		self->getOnChange()(self);
		};
	std::function<void(Base*, char)> getKeyHandler() {
		return onKeyPress;
	}

	void modifyText(const char* str) {
		innerText->value = (char*)str;
	}
	void draw(int scrollX, int scrollY) {
		Base::draw(scrollX, scrollY);

	}

	Text* getInnerText() {
		return innerText;
	}
	//CHECK FOR NULL;
	std::function<void(Base*)>& getOnChange() {
		return onChange;
	};


};

class MDui
{
public:
	int height;
	int width;
	int scrollY;
	int scrollX;
	bool overflowX, overflowY;
	char pressedChar;
	V2 mousePos;
	bool mouseClicked;
	int focus = -1;
	double lastBackSpace = 0;
	int maxWidth, maxHeight;
	std::vector<Base*> children;
	std::function<void(MDui&)> onClick = [&](MDui& self)->void {};
	std::function<void(MDui&, char)> onKeyPress = [&](MDui& self, char c)->void {};
	MDui(int height, int width) :height(height), width(width), overflowX(false), overflowY(false), focus(-1), pressedChar(0), mousePos({ 0,0 }), maxWidth(0), maxHeight(0) {}
	MDui& addChild(Base& child, bool fill = true) {
		children.push_back(&child);

		return *this;
	}
	void init(const char* title) {
		InitWindow(width, height, title);

	}
	void draw() {
		for (size_t i = 0; i < children.size(); i++) {
			Base* child = children[i];

			child->draw(scrollX, scrollY);

		}
	}


	void programLoop(std::function<void(void)> mainFunc) {
		while (!WindowShouldClose()) {
			BeginDrawing();
			ClearBackground(RAYWHITE);
			pressedChar = GetCharPressed();
			mousePos = {
				GetMouseX(),
				GetMouseY()
			};
			mouseClicked = IsMouseButtonPressed(0);
			if (mouseClicked) {
				updateFocus(mousePos);
				if (focus > -1) {
					Base* clickedEl = focusedEl();

					clickedEl->getOnClick()(clickedEl);
				}
				else {
					onClick(*this);
				}
			}
			int x = GetMouseWheelMove();
			if (x != 0) {
				int maxHeight = getMaxHeight();
				int overflowYAmt = maxHeight - height;
				int scrollAmt = 10;
				scrollY += (x * scrollAmt);
				if ((-scrollY) > overflowYAmt) {
					scrollY -= (x * scrollAmt);
				}
				if ((scrollY) > 0) {
					scrollY -= (x * scrollAmt);
				}
			}


			if (pressedChar != 0) {
				if (focus > -1) {
					Base* clickedEl = focusedEl();
					clickedEl->getKeyHandler()(clickedEl, pressedChar);
				}
				else {
					onKeyPress(*this, pressedChar);
				}
			}
			else {
				if (IsKeyDown(KEY_BACKSPACE)) {
					if (GetTime() - lastBackSpace > 0.1) {
						int backSpaceAscii = 8;
						if (focus > -1) {
							Base* clickedEl = focusedEl();
							clickedEl->getKeyHandler()(clickedEl, backSpaceAscii);
						}
						else {
							onKeyPress(*this, backSpaceAscii);
						}
						lastBackSpace = GetTime();
					}
				}
			}
			mainFunc();
			EndDrawing();
		}
	}

	void updateFocus(V2 mousePos) {
		int mouseX = mousePos.x;
		int mouseY = mousePos.y;
		for (size_t i = 0; i < children.size(); i++) {
			Base* child = children[i];
			Rectangle childRect = child->getComputedRect();
			if (CheckCollisionPointRec({ (float)mouseX,(float)mouseY }, childRect)) {
				focus = i;
				child->updateFocus(mousePos);
				return;
			}
		}
		focus = -1;
	}

	int getMaxHeight() {

		int maxHeight = height;
		if (children.size() > 0) {
			for (size_t i = 0; i < children.size(); i++) {
				Base& child = *children[i];
				int childMaxHeight = child.getMaxHeight();
				if (childMaxHeight > maxHeight) {
					maxHeight = childMaxHeight;
				}
			}
		}
		return maxHeight;
	}


	Base* focusedEl() {
		if (focus > -1) {
			Base* f = children[focus]->focusedEl();
			return f;
		}
	}
};



