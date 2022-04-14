#pragma once
#include <string>
using namespace std;

#pragma endregion

enum LabelStyles {
	LABEL_BIG,
	LABEL_SMALL
};

enum SpacerTypes
{
	SPACER_BIG,
	SPACER_SMALL
};

enum CheckboxTypes
{
	CHECKBOX_SELECTED,
	CHECKBOX_NOT_SELECTED
};
#pragma region Dialog stuff
class GTDialog
{
public:
	string dialogstr = "";
	void addSpacer(SpacerTypes type);
	void addLabelWithIcon(string text, int tileid, LabelStyles type);
	void addButton(string buttonname, string buttontext);
	void addCheckbox(string checkboxname, string string, CheckboxTypes type);
	void addTextBox(string str);
	void addSmallText(string str);
	void addInputBox(string name, string text, string cont, int size);
	void addQuickExit();
	void endDialog(string name, string accept, string nvm);
	void addCustom(string name);
	string finishDialog();

	operator string() {
		return this->dialogstr;
	}
};