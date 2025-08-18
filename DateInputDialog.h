#ifndef DATE_INPUT_DIALOG_H
#define DATE_INPUT_DIALOG_H

#include <Button.h>
#include <String.h>
#include <TextControl.h>
#include <Window.h>

class DateInputDialog : public BWindow {
public:
	DateInputDialog(BWindow* parent, const char* title, const char* initialDate = NULL);
	virtual ~DateInputDialog();

	virtual void MessageReceived(BMessage* msg);
	const char* GetDate() const { return fDate.String(); }

private:
	void _BuildUI();

	BWindow* fParent;
	BTextControl* fDateText;
	BButton* fOKButton;
	BButton* fCancelButton;
	BString fDate;

	static const uint32 kMsgOK = 'ok  ';
	static const uint32 kMsgCancel = 'cncl';
};

#endif // DATE_INPUT_DIALOG_H
