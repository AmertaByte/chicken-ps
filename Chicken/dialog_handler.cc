#include <iostream>
#include "dialog_handler.hh"

void GTDialog::addSpacer(SpacerTypes type) {
	switch (type) {
	case SPACER_BIG:
		this->dialogstr.append("add_spacer|big|\n");
		break;
	case SPACER_SMALL:
		this->dialogstr.append("add_spacer|small|\n");
		break;
	default:
		break;
	}
}

void GTDialog::addLabelWithIcon(string text, int tileid, LabelStyles type) {
	switch (type) {
	case LABEL_BIG:
		this->dialogstr.append("add_label_with_icon|big|" + text + "|left|" + to_string(tileid) + "|\n");
		break;
	case LABEL_SMALL:
		this->dialogstr.append("add_label_with_icon|small|" + text + "|left|" + to_string(tileid) + "|\n");
		break;
	default:
		break;
	}
}

void GTDialog::addButton(string buttonname, string buttontext) {
	this->dialogstr.append("add_button|" + buttonname + "|" + buttontext + "|noflags|0|0|\n");
}

void GTDialog::addCheckbox(string checkboxname, string string, CheckboxTypes type) {
	switch (type) {
	case CHECKBOX_SELECTED:
		this->dialogstr.append("add_checkbox|" + checkboxname + "|" + string + "|1|\n");
		break;
	case CHECKBOX_NOT_SELECTED:
		this->dialogstr.append("add_checkbox|" + checkboxname + "|" + string + "|0|\n");
		break;
	default:
		break;
	}
}

void GTDialog::addTextBox(string str) {
	this->dialogstr.append("add_textbox|" + str + "|left|\n");
}

void GTDialog::addSmallText(string str) {
	this->dialogstr.append("add_smalltext|" + str + "|\n");
}

void GTDialog::addInputBox(string name, string text, string cont, int size) {
	this->dialogstr.append("add_text_input|" + name + "|" + text + "|" + cont + "|" + to_string(size) + "|\n");
}

void GTDialog::addQuickExit() {
	this->dialogstr.append("add_quick_exit|\n");
}

void GTDialog::endDialog(string name, string accept, string nvm) {
	this->dialogstr.append("end_dialog|" + name + "|" + nvm + "|" + accept + "|\n");
}

void GTDialog::addCustom(string name) {
	this->dialogstr.append(name + "\n");
}

string GTDialog::finishDialog() {
	return this->dialogstr;
}
#pragma endregion