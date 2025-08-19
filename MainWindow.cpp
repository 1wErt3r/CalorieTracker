#include "MainWindow.h"
#include "DateInputDialog.h"

#include <Alert.h>
#include <Application.h>
#include <Button.h>
#include <Catalog.h>
#include <File.h>
#include <FilePanel.h>
#include <FindDirectory.h>
#include <LayoutBuilder.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Path.h>
#include <PopUpMenu.h>
#include <ScrollView.h>
#include <StringItem.h>
#include <StringView.h>
#include <TextControl.h>
#include <View.h>
#include <private/interface/ColumnListView.h>
#include <private/interface/ColumnTypes.h>
#include <time.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Window"

static const uint32 kMsgAddFood = 'fadd';
static const uint32 kMsgRemoveFood = 'frem';
static const uint32 kMsgFoodSelectionChanged = 'fsel';
static const uint32 kMsgDayChanged = 'dych';
static const uint32 kMsgNewDay = 'nday';
static const uint32 kMsgRemoveDay = 'rday';
static const uint32 kMsgEditFood = 'edit';
static const uint32 kMsgCustomDateOK = 'ok  ';
static const uint32 kMsgCustomDateCancel = 'cncl';
static const uint32 kMsgExportCSV = 'csv ';
static const uint32 kMsgSaveCSV = 'scsv';


MainWindow::MainWindow()
	:
	BWindow(BRect(100, 100, 900, 650), B_TRANSLATE("CalorieTracker"), B_TITLED_WINDOW,
		B_ASYNCHRONOUS_CONTROLS | B_QUIT_ON_WINDOW_CLOSE),
	fCurrentDay("")
{
	BLayoutBuilder::Group<>(this, B_VERTICAL).Add(_BuildMenuBar()).Add(_BuildMainView()).End();

	fFilePanel = new BFilePanel(B_SAVE_PANEL, new BMessenger(this), NULL, 0, false,
		new BMessage(kMsgSaveCSV));

	_LoadData();
	_UpdateDayPicker();
	_UpdateDailyFoodList();
	_UpdateTotalCalories();

	// Set initial focus
	fFoodNameText->MakeFocus();
}


BMenuBar*
MainWindow::_BuildMenuBar()
{
	BMenuBar* menuBar = new BMenuBar("menubar");

	BMenu* fileMenu = new BMenu(B_TRANSLATE("Calorie Tracker"));
	fileMenu->AddItem(new BMenuItem(B_TRANSLATE("New day"), new BMessage(kMsgNewDay), 'N'));
	fileMenu->AddItem(new BMenuItem(B_TRANSLATE("CSV Export"), new BMessage(kMsgExportCSV), 'E'));
	fileMenu->AddSeparatorItem();
	fileMenu->AddItem(new BMenuItem(B_TRANSLATE("Quit"), new BMessage(B_QUIT_REQUESTED), 'Q'));

	BMenu* editMenu = new BMenu(B_TRANSLATE("Edit"));
	editMenu->AddItem(new BMenuItem(B_TRANSLATE("Add food"), new BMessage(kMsgAddFood), 'A'));
	editMenu->AddItem(
		new BMenuItem(B_TRANSLATE("Remove selected food"), new BMessage(kMsgRemoveFood), 'R'));

	menuBar->AddItem(fileMenu);
	menuBar->AddItem(editMenu);

	return menuBar;
}


BView*
MainWindow::_BuildMainView()
{
	// Create the main view
	BView* mainView = new BView("mainView", B_WILL_DRAW);
	mainView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	// Create UI elements
	fFoodNameText
		= new BTextControl("foodName", B_TRANSLATE("Food name:"), "", new BMessage(kMsgAddFood));
	fFoodNameText->SetDivider(be_plain_font->StringWidth(B_TRANSLATE("Food name:")) + 10);

	fCaloriesText
		= new BTextControl("calories", B_TRANSLATE("Calories:"), "", new BMessage(kMsgAddFood));
	fCaloriesText->SetDivider(be_plain_font->StringWidth(B_TRANSLATE("Calories:")) + 10);

	// Create day picker with better layout
	fDayPicker = new BMenuField("dayPicker", B_TRANSLATE("Selected day:"),
		new BPopUpMenu(B_TRANSLATE("No days available")));
	fDayPicker->SetDivider(be_plain_font->StringWidth(B_TRANSLATE("Selected day:")) + 10);

	// Create buttons with better styling
	fAddButton = new BButton("addButton", B_TRANSLATE("Add Food"), new BMessage(kMsgAddFood));
	fAddButton->MakeDefault(true);

	fRemoveButton
		= new BButton("removeButton", B_TRANSLATE("Remove Selected"), new BMessage(kMsgRemoveFood));
	fRemoveButton->SetEnabled(false);

	fNewDayButton = new BButton("newDayButton", B_TRANSLATE("New Day"), new BMessage(kMsgNewDay));

	fRemoveDayButton
		= new BButton("removeDayButton", B_TRANSLATE("Remove Day"), new BMessage(kMsgRemoveDay));
	fRemoveDayButton->SetEnabled(false);

	fEditFoodButton
		= new BButton("editFoodButton", B_TRANSLATE("Edit Food"), new BMessage(kMsgEditFood));
	fEditFoodButton->SetEnabled(false);


	fDailyFoodListView = new BColumnListView("dailyFoodList", B_NAVIGABLE, B_FANCY_BORDER, true);
	fDailyFoodListView->AddColumn(
		new BStringColumn(B_TRANSLATE("Food"), 200, 100, 400, B_TRUNCATE_END), 0);
	fDailyFoodListView->AddColumn(new BIntegerColumn(B_TRANSLATE("Calories"), 100, 80, 150), 1);
	fDailyFoodListView->SetSortingEnabled(true);

	// Create status views
	fTotalCaloriesView = new BStringView("totalCalories", B_TRANSLATE("Total calories: 0"));
	fTotalCaloriesView->SetFont(be_bold_font);

	fDayStatusView = new BStringView("dayStatus", B_TRANSLATE("No day selected"));
	fDayStatusView->SetFont(be_plain_font);

	// Layout the UI with better organization
	BLayoutBuilder::Group<>(mainView, B_VERTICAL)

		// Day management section
		.AddGroup(B_VERTICAL, B_USE_SMALL_SPACING)
		.Add(new BStringView("dayLabel", B_TRANSLATE("Day Management")))
		.Add(fDayPicker)
		.AddGroup(B_HORIZONTAL, B_USE_DEFAULT_SPACING)
		.Add(fNewDayButton)
		.Add(fRemoveDayButton)
		.Add(fDayStatusView)
		.AddGlue()
		.End()
		.End()

		// Input section


		// Data views section
		.AddGroup(B_HORIZONTAL, B_USE_DEFAULT_SPACING)
		// Unique foods panel


		// Daily foods panel
		.AddGroup(B_VERTICAL, B_USE_SMALL_SPACING)
		.Add(new BStringView("dailyFoodsLabel", B_TRANSLATE("Today's Food Intake")))
		.Add(fDailyFoodListView)
		.Add(fTotalCaloriesView)
		.End()
		.End()

		.AddGroup(B_VERTICAL, B_USE_SMALL_SPACING)
		.Add(new BStringView("inputLabel", B_TRANSLATE("Add Food Entry")))
		.AddGrid(B_USE_DEFAULT_SPACING, B_USE_SMALL_SPACING)
		.Add(fFoodNameText->CreateLabelLayoutItem(), 0, 0)
		.Add(fFoodNameText->CreateTextViewLayoutItem(), 1, 0)
		.Add(fCaloriesText->CreateLabelLayoutItem(), 0, 1)
		.Add(fCaloriesText->CreateTextViewLayoutItem(), 1, 1)
		.End()
		.AddGroup(B_HORIZONTAL, B_USE_DEFAULT_SPACING)
		.Add(fAddButton)
		.Add(fRemoveButton)
		.AddGlue()
		.End()
		.End()
		.SetInsets(B_USE_DEFAULT_SPACING);

	return mainView;
}


void
MainWindow::_LoadData()
{
	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
		path.Append("CalorieTracker_data");
		BFile file(path.Path(), B_READ_ONLY);
		if (file.InitCheck() == B_OK)
			fFoodData.Unflatten(&file);
	}

	// If no data was loaded, initialize empty structure
	if (fFoodData.IsEmpty()) {
		// Start with today's date
		time_t now = time(NULL);
		struct tm* timeinfo = localtime(&now);
		char dateStr[11];
		strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", timeinfo);
		fFoodData.AddString("days", dateStr);
		_SaveData(); // Save the initial data
	}
}


void
MainWindow::_SaveData()
{
	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
		path.Append("CalorieTracker_data");
		BFile file(path.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		if (file.InitCheck() == B_OK)
			fFoodData.Flatten(&file);
	}
}


MainWindow::~MainWindow()
{
	_SaveData();
	delete fFilePanel;
}


void
MainWindow::_UpdateTotalCalories()
{
	int32 totalCalories = 0;

	if (fCurrentDay.IsEmpty()) {
		fTotalCaloriesView->SetText(B_TRANSLATE("Total calories: 0"));
		return;
	}

	BMessage entry;
	for (int32 i = 0; fFoodData.FindMessage("entries", i, &entry) == B_OK; i++) {
		const char* entryDate;
		if (entry.FindString("date", &entryDate) == B_OK && fCurrentDay.Compare(entryDate) == 0) {
			int32 calories;
			if (entry.FindInt32("calories", &calories) == B_OK)
				totalCalories += calories;
		}
	}

	BString totalCaloriesStr;
	totalCaloriesStr << B_TRANSLATE("Total calories: ") << totalCalories;
	fTotalCaloriesView->SetText(totalCaloriesStr.String());
}


void
MainWindow::_UpdateDailyFoodList()
{
	fDailyFoodListView->Clear();

	if (fCurrentDay.IsEmpty()) {
		fRemoveButton->SetEnabled(false);
		return;
	}

	BMessage entry;
	for (int32 i = 0; fFoodData.FindMessage("entries", i, &entry) == B_OK; i++) {
		const char* entryDate;
		if (entry.FindString("date", &entryDate) == B_OK && fCurrentDay.Compare(entryDate) == 0) {
			const char* name;
			int32 calories;
			if (entry.FindString("name", &name) == B_OK
				&& entry.FindInt32("calories", &calories) == B_OK) {
				BRow* row = new BRow();
				row->SetField(new BStringField(name), 0);
				row->SetField(new BIntegerField(calories), 1);
				fDailyFoodListView->AddRow(row);
			}
		}
	}

	fRemoveButton->SetEnabled(fDailyFoodListView->CountRows() > 0);
}


void
MainWindow::_ExportCSV()
{
	fFilePanel->Show();
}


void
MainWindow::_SaveCSV(BMessage* message)
{
	entry_ref ref;
	if (message->FindRef("directory", &ref) != B_OK)
		return;

	const char* name;
	if (message->FindString("name", &name) != B_OK)
		return;

	BPath path(&ref);
	path.Append(name);

	BFile file(path.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	if (file.InitCheck() != B_OK) {
		BAlert* alert = new BAlert(B_TRANSLATE("Export Error"),
			B_TRANSLATE("Could not create CSV file."), B_TRANSLATE("OK"));
		alert->Go();
		return;
	}

	// Write CSV header
	BString header("Food,Calories,Date\n");
	file.Write(header.String(), header.Length());

	// Write all entries for all days
	const char* date;
	for (int32 i = 0; fFoodData.FindString("days", i, &date) == B_OK; i++) {
		BMessage entry;
		for (int32 j = 0; fFoodData.FindMessage("entries", j, &entry) == B_OK; j++) {
			const char* entryDate;
			if (entry.FindString("date", &entryDate) == B_OK && strcmp(date, entryDate) == 0) {
				const char* foodName;
				int32 calories;

				if (entry.FindString("name", &foodName) == B_OK
					&& entry.FindInt32("calories", &calories) == B_OK) {

					// Escape quotes in food name for CSV
					BString escapedName(foodName);
					escapedName.ReplaceAll("\"", "\"\"");

					BString csvLine;
					csvLine << "\"" << escapedName.String() << "," << calories << ","
							<< "\"" << date << "\"\n";
					file.Write(csvLine.String(), csvLine.Length());
				}
			}
		}
	}

	BString successMsg;
	successMsg << B_TRANSLATE("CSV exported successfully to:\n") << path.Path();
	BAlert* alert
		= new BAlert(B_TRANSLATE("Export Complete"), successMsg.String(), B_TRANSLATE("OK"));
	alert->Go();
}


void
MainWindow::_UpdateDayPicker()
{
	BMenu* menu = fDayPicker->Menu();

	// Clear existing items
	for (int32 i = menu->CountItems() - 1; i >= 0; i--) {
		BMenuItem* item = menu->RemoveItem(i);
		delete item;
	}

	// Add days in reverse chronological order (newest first)
	BList daysList;
	const char* date;
	for (int32 i = 0; fFoodData.FindString("days", i, &date) == B_OK; i++)
		daysList.AddItem((void*)date);

	// Sort days in descending order
	daysList.SortItems(
		[](const void* a, const void* b) -> int { return strcmp((const char*)b, (const char*)a); });

	if (daysList.CountItems() == 0) {
		menu->AddItem(new BMenuItem(B_TRANSLATE("No days available"), NULL));
		fCurrentDay = "";
		fDayStatusView->SetText(B_TRANSLATE("No day selected"));
		fRemoveDayButton->SetEnabled(false);
	} else {
		for (int32 i = 0; i < daysList.CountItems(); i++) {
			const char* dayStr = (const char*)daysList.ItemAt(i);
			BMenuItem* item = new BMenuItem(dayStr, new BMessage(kMsgDayChanged));
			menu->AddItem(item);
			if (i == 0 || (fCurrentDay.IsEmpty() == false && fCurrentDay.Compare(dayStr) == 0)) {
				item->SetMarked(true);
				fCurrentDay = dayStr;
			}
		}

		if (fCurrentDay.IsEmpty()) {
			fCurrentDay = (const char*)daysList.ItemAt(0);
			menu->ItemAt(0)->SetMarked(true);
		}

		BString statusStr;
		statusStr << B_TRANSLATE("Selected: ") << fCurrentDay;
		fDayStatusView->SetText(statusStr.String());
		fRemoveDayButton->SetEnabled(true);
	}
}


bool
MainWindow::_ValidateInput(const char* foodName, const char* caloriesText, BString& errorMsg)
{
	if (!foodName || strlen(foodName) == 0) {
		errorMsg = B_TRANSLATE("Please enter a food name.");
		return false;
	}

	if (strlen(foodName) > 100) {
		errorMsg = B_TRANSLATE("Food name is too long (max 100 characters).");
		return false;
	}

	if (!caloriesText || strlen(caloriesText) == 0) {
		errorMsg = B_TRANSLATE("Please enter calorie amount.");
		return false;
	}

	int32 calories = atoi(caloriesText);
	if (calories <= 0 || calories > 9999) {
		errorMsg = B_TRANSLATE("Please enter a valid calorie amount (1-9999).");
		return false;
	}

	if (fCurrentDay.IsEmpty()) {
		errorMsg = B_TRANSLATE("Please select or create a day first.");
		return false;
	}

	return true;
}


void
MainWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgAddFood:
		{
			const char* foodName = fFoodNameText->Text();
			const char* caloriesText = fCaloriesText->Text();

			BString errorMsg;
			if (!_ValidateInput(foodName, caloriesText, errorMsg)) {
				BAlert* alert
					= new BAlert(B_TRANSLATE("Input Error"), errorMsg.String(), B_TRANSLATE("OK"));
				alert->Go();
				break;
			}

			int32 calories = atoi(caloriesText);

			// Add food entry
			BMessage foodEntry;
			foodEntry.AddString("name", foodName);
			foodEntry.AddInt32("calories", calories);
			foodEntry.AddString("date", fCurrentDay.String());
			fFoodData.AddMessage("entries", &foodEntry);

			// Clear input fields
			fFoodNameText->SetText("");
			fCaloriesText->SetText("");
			fFoodNameText->MakeFocus();

			_UpdateDailyFoodList();
			_UpdateTotalCalories();
			_SaveData();
		} break;

		case kMsgRemoveFood:
		{
			BRow* row = fDailyFoodListView->CurrentSelection();
			if (row == NULL)
				break;

			BStringField* nameField = (BStringField*)row->GetField(0);
			BIntegerField* caloriesField = (BIntegerField*)row->GetField(1);

			const char* foodName = nameField->String();
			int32 calories = caloriesField->Value();

			// Find and remove the matching entry
			BMessage entry;
			for (int32 i = 0; fFoodData.FindMessage("entries", i, &entry) == B_OK; i++) {
				const char* name;
				const char* entryDate;
				int32 entryCals;

				if (entry.FindString("name", &name) == B_OK
					&& entry.FindString("date", &entryDate) == B_OK
					&& entry.FindInt32("calories", &entryCals) == B_OK
					&& strcmp(name, foodName) == 0 && strcmp(entryDate, fCurrentDay.String()) == 0
					&& entryCals == calories) {
					fFoodData.RemoveData("entries", i);
					break;
				}
			}

			_UpdateDailyFoodList();
			_UpdateTotalCalories();
			_SaveData();
		} break;

		case kMsgDayChanged:
		{
			BMenuItem* item = NULL;
			if (message->FindPointer("source", (void**)&item) == B_OK) {
				fCurrentDay = item->Label();
				BString statusStr;
				statusStr << B_TRANSLATE("Selected: ") << fCurrentDay;
				fDayStatusView->SetText(statusStr.String());
			}

			_UpdateDailyFoodList();
			_UpdateTotalCalories();
		} break;

		case kMsgNewDay:
		{
			BAlert* alert
				= new BAlert(B_TRANSLATE("New Day"), B_TRANSLATE("Enter date (YYYY-MM-DD):"),
					B_TRANSLATE("Today"), B_TRANSLATE("Custom"), B_TRANSLATE("Cancel"));

			int32 result = alert->Go();

			if (result == 2) // Cancel
				break;

			BString newDate;
			if (result == 0) { // Today
				time_t now = time(NULL);
				struct tm* timeinfo = localtime(&now);
				char dateStr[11];
				strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", timeinfo);
				newDate = dateStr;

				// Check if date already exists
				bool exists = false;
				const char* existingDate;
				for (int32 i = 0; fFoodData.FindString("days", i, &existingDate) == B_OK; i++) {
					if (newDate.Compare(existingDate) == 0) {
						exists = true;
						break;
					}
				}

				if (!exists) {
					fFoodData.AddString("days", newDate.String());
					_UpdateDayPicker();
					_SaveData();
				} else {
					BAlert* existsAlert = new BAlert(B_TRANSLATE("Day Exists"),
						B_TRANSLATE("This day already exists in the database."), B_TRANSLATE("OK"));
					existsAlert->Go();
				}
			} else { // Custom
				// Show our new date input dialog
				DateInputDialog* dialog = new DateInputDialog(this, B_TRANSLATE("Custom Date"));
				dialog->Show();
			}
		} break;

		case kMsgCustomDateOK:
		{
			// Handle the custom date entered by the user
			const char* dateString;
			if (message->FindString("date", &dateString) == B_OK) {
				BString newDate(dateString);

				// Basic validation - check if it looks like a valid date format
				if (newDate.Length() == 10 && newDate[4] == '-' && newDate[7] == '-') {
					// Check if date already exists
					bool exists = false;
					const char* existingDate;
					for (int32 i = 0; fFoodData.FindString("days", i, &existingDate) == B_OK; i++) {
						if (newDate.Compare(existingDate) == 0) {
							exists = true;
							break;
						}
					}

					if (!exists) {
						fFoodData.AddString("days", newDate.String());
						_UpdateDayPicker();
						_SaveData();
					} else {
						BAlert* existsAlert = new BAlert(B_TRANSLATE("Day Exists"),
							B_TRANSLATE("This day already exists in the database."),
							B_TRANSLATE("OK"));
						existsAlert->Go();
					}
				} else {
					BAlert* errorAlert = new BAlert(B_TRANSLATE("Invalid Date"),
						B_TRANSLATE("Please enter a valid date in YYYY-MM-DD format."),
						B_TRANSLATE("OK"));
					errorAlert->Go();
				}
			}
		} break;

		case kMsgCustomDateCancel:
		{
			// User cancelled the custom date dialog - do nothing
		} break;

		case kMsgRemoveDay:
		{
			if (fCurrentDay.IsEmpty())
				break;

			BString alertText;
			alertText << B_TRANSLATE("Are you sure you want to remove the day '") << fCurrentDay
					  << B_TRANSLATE("' and all its food entries?");

			BAlert* alert = new BAlert(B_TRANSLATE("Remove Day"), alertText.String(),
				B_TRANSLATE("Remove"), B_TRANSLATE("Cancel"));
			alert->SetShortcut(1, B_ESCAPE);

			if (alert->Go() == 0) {
				// Remove the day
				const char* date;
				for (int32 i = 0; fFoodData.FindString("days", i, &date) == B_OK; i++) {
					if (fCurrentDay.Compare(date) == 0) {
						fFoodData.RemoveData("days", i);
						break;
					}
				}

				// Remove all entries for this day
				BMessage entry;
				for (int32 i = fFoodData.CountNames(B_MESSAGE_TYPE) - 1; i >= 0; i--) {
					if (fFoodData.FindMessage("entries", i, &entry) == B_OK) {
						const char* entryDate;
						if (entry.FindString("date", &entryDate) == B_OK
							&& fCurrentDay.Compare(entryDate) == 0) {
							fFoodData.RemoveData("entries", i);
						}
					}
				}

				fCurrentDay = "";
				_UpdateDayPicker();
				_UpdateDailyFoodList();
				_UpdateTotalCalories();
				_SaveData();
			}
		} break;


		case kMsgEditFood:
		{
			int32 selection = fFoodListView->CurrentSelection();
			if (selection < 0)
				break;

			BStringItem* item = (BStringItem*)fFoodListView->ItemAt(selection);
			const char* foodName = item->Text();

			// Find the food in unique_foods and get its calories
			BMessage uniqueFood;
			int32 currentCalories = 0;
			for (int32 i = 0; fFoodData.FindMessage("unique_foods", i, &uniqueFood) == B_OK; i++) {
				const char* name;
				if (uniqueFood.FindString("name", &name) == B_OK && strcmp(name, foodName) == 0) {

					uniqueFood.FindInt32("calories", &currentCalories);
					break;
				}
			}

			BString alertText;
			alertText << B_TRANSLATE("Current calories for '") << foodName
					  << "': " << currentCalories;

			BAlert* alert
				= new BAlert(B_TRANSLATE("Edit Food"), alertText.String(), B_TRANSLATE("OK"));
			alert->Go();
			// Note: In a real implementation, you'd want a proper edit dialog
		} break;

		case kMsgExportCSV:
		{
			_ExportCSV();
		} break;

		case kMsgSaveCSV:
		{
			_SaveCSV(message);
		} break;

		case kMsgFoodSelectionChanged:
		{
			int32 selection = fFoodListView->CurrentSelection();
			fEditFoodButton->SetEnabled(selection >= 0);

			if (selection < 0)
				break;

			BStringItem* item = (BStringItem*)fFoodListView->ItemAt(selection);
			const char* foodName = item->Text();

			// Auto-fill the food name and calories
			BMessage uniqueFood;
			for (int32 i = 0; fFoodData.FindMessage("unique_foods", i, &uniqueFood) == B_OK; i++) {
				const char* name;
				if (uniqueFood.FindString("name", &name) == B_OK && strcmp(name, foodName) == 0) {
					int32 calories;
					if (uniqueFood.FindInt32("calories", &calories) == B_OK) {
						fFoodNameText->SetText(name);
						BString calStr;
						calStr << calories;
						fCaloriesText->SetText(calStr.String());
						fCaloriesText->MakeFocus();
					}
					break;
				}
			}
		} break;

		default:
		{
			BWindow::MessageReceived(message);
			break;
		}
	}
}
