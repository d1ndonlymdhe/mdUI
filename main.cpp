#include<raylib.h>
#include<MDui.h>

void drawGrid(MDui root) {
	for (auto i = 0; i < root.width; i += 10) {
		if (i % 100 == 0) {
			DrawLine(i, 0, i, root.height, RED);
		}
		else {
			DrawLine(i, 0, i, root.height, ColorAlpha(BLACK, 0.7));

		}
	}
	for (auto i = 0; i < root.height; i += 10) {
		if (i % 100 == 0) {
			DrawLine(0, i, root.width, i, RED);
		}
		else {
			DrawLine(0, i, root.width, i, ColorAlpha(BLACK, 0.7));

		}
	}
}


int main() {
	MDui root(1000, 1000);
	root.init("Nice");
	
	Text headerText((char*)"Welcome to MDUI demo", 40, BLACK);
	Base header(0, 0, WHITE);
	header.addText(&headerText);

	Base inputBase(0, 0, RED);
	inputBase.updatePadding({ 20,40 });

	Input nameInput;
	nameInput.updatePadding({ 10,80 }).updateBg(WHITE);
	

	inputBase.addChild(nameInput);
	

	Text outputText((char*)"", 40, WHITE);

	nameInput.getOnChange() = [&](Base* self)->void {
		outputText.value = (char*)self->getValue().c_str();
	};


	Base outputBase(0, 0, BLUE);
	outputBase.addText(&outputText);


	int rowTemp[3] = {20,20,60};
	int colTemp[1] = { 100 };
	Grid mainGrid(3, 1, rowTemp, colTemp, 0, 0, root.height, root.width, RAYWHITE);

	mainGrid.addChild(header);
	mainGrid.addChild(inputBase);
	mainGrid.addChild(outputBase);


	root.addChild(mainGrid);

	root.programLoop([&]()->void {
		root.draw();
	});
}