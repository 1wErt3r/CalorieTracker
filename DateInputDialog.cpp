#include "DateInputDialog.h"

#include <Catalog.h>
#include <LayoutBuilder.h>
#include <String.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "DateInputDialog"


DateInputDialog::DateInputDialog(BWindow* parent, const char* title, const char* initialDate)
	:
	BWindow(BRect(0, 0, 400, 200), title, B_TITLED_WINDOW,
		B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE | B_NOT_ZOOMABLE),
	fParent(parent),
	fDateText(NULL),
	fOKButton(NULL),
	fCancelButton(NULL)
{
	_BuildUI();

	if (initialDate != NULL)
		fDateText->SetText(initialDate);

	// Center window on parent
	if (parent) {
		BRect parentFrame = parent->Frame();
		BRect myFrame = Frame();
		MoveTo(parentFrame.left + (parentFrame.Width() - myFrame.Width()) / 2,
			parentFrame.top + (parentFrame.Height() - myFrame.Height()) / 2);
	}

	// Make the date text control the default focus
	fDateText->MakeFocus(true);
}


DateInputDialog::~DateInputDialog()
{
}


void
DateInputDialog::_BuildUI()
{
	fDateText
		= new BTextControl("dateText", B_TRANSLATE("Date (YYYY-MM-DD):"), "", new BMessage(kMsgOK));
	fDateText->SetDivider(be_plain_font->StringWidth(B_TRANSLATE("Date (YYYY-MM-DD):")) + 10);

	fOKButton = new BButton("okButton", B_TRANSLATE("OK"), new BMessage(kMsgOK));
	fCancelButton = new BButton("cancelButton", B_TRANSLATE("Cancel"), new BMessage(kMsgCancel));

	// Make OK button the default
	fOKButton->MakeDefault(true);

	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.SetInsets(B_USE_DEFAULT_SPACING)
		.Add(fDateText)
		.AddGroup(B_HORIZONTAL)
		.AddGlue()
		.Add(fCancelButton)
		.Add(fOKButton)
		.End()
		.End();
}


void
DateInputDialog::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
		case kMsgOK:
		{
			fDate = fDateText->Text();
			if (fParent) {
				BMessage reply(kMsgOK);
				reply.AddString("date", fDate);
				fParent->PostMessage(&reply);
			}
			Quit();
		} break;

		case kMsgCancel:
		{
			if (fParent) {
				BMessage reply(kMsgCancel);
				fParent->PostMessage(&reply);
			}
			Quit();
		} break;

		default:
			BWindow::MessageReceived(msg);
			break;
	}
}
