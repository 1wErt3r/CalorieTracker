#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <Button.h>
#include <ColumnListView.h>
#include <ListView.h>
#include <MenuField.h>
#include <SplitView.h>
#include <StringView.h>
#include <TextControl.h>
#include <Window.h>

class BFilePanel;
class DateInputDialog;


class MainWindow : public BWindow {
public:
	MainWindow();
	virtual ~MainWindow();

	virtual void MessageReceived(BMessage* msg);

private:
	BMenuBar* _BuildMenuBar();
	BView* _BuildMainView();

	void _UpdateTotalCalories();
	void _UpdateDailyFoodList();
	void _LoadData();
	void _SaveData();
	void _UpdateDayPicker();
	bool _ValidateInput(const char* foodName, const char* caloriesText, BString& errorMsg);
	void _ExportCSV();
	void _SaveCSV(BMessage* message);

	BTextControl* fFoodNameText;
	BTextControl* fCaloriesText;
	BMenuField* fDayPicker;
	BButton* fAddButton;
	BButton* fRemoveButton;
	BButton* fNewDayButton;
	BButton* fRemoveFoodButton;
	BButton* fRemoveDayButton;
	BButton* fEditFoodButton;
	BListView* fFoodListView;
	BColumnListView* fDailyFoodListView;
	BStringView* fTotalCaloriesView;
	BStringView* fDayStatusView;
	BString fCurrentDay;
	BFilePanel* fFilePanel;

	BMessage fFoodData;
};

#endif
