
#include "CognitiveVREditorPrivatePCH.h"
#include "CognitiveTools.h"

#define LOCTEXT_NAMESPACE "BaseToolEditor"

//deals with all the interface stuff in the editor preferences
//includes any details needed to make the ui work

void FCognitiveTools::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TSet<UClass*> Classes;

	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);

	DetailLayoutPtr = &DetailBuilder;

	UClass* Class = NULL;

	for (auto WeakObject : ObjectsBeingCustomized)
	{
		if (UObject* Instance = WeakObject.Get())
		{
			Class = Instance->GetClass();
			break;
		}
	}


	IDetailCategoryBuilder& SettingsCategory = DetailBuilder.EditCategory(TEXT("Export Settings"));

	MinPolygonProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UCognitiveVRSettings, MinPolygons));
	MaxPolygonProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UCognitiveVRSettings, MaxPolygons));
	StaticOnlyProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UCognitiveVRSettings, staticOnly));
	MinSizeProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UCognitiveVRSettings, MinimumSize));
	MaxSizeProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UCognitiveVRSettings, MaximumSize));
	TextureResizeProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UCognitiveVRSettings, TextureResizeFactor));
	ExcludeMeshProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UCognitiveVRSettings, ExcludeMeshes));

	// Create a commands category
	IDetailCategoryBuilder& LoginCategory = DetailBuilder.EditCategory(TEXT("Account"),FText::GetEmpty(),ECategoryPriority::Important);

	LoginCategory.AddCustomRow(FText::FromString("Commands"))
		.ValueContent()
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
				//button
			+ SHorizontalBox::Slot()
			.MaxWidth(96)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Email"))
			]

			+ SHorizontalBox::Slot()
			.MaxWidth(128)
			//.Padding(4)
			[
				SNew(SEditableTextBox)
				.MinDesiredWidth(128)
				.Visibility(this,&FCognitiveTools::LoginTextboxUsable)
				.OnTextChanged(this,&FCognitiveTools::OnEmailChanged)
			]
		];

	LoginCategory.AddCustomRow(FText::FromString("Commands"))
		.ValueContent()
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			//button
			+ SHorizontalBox::Slot()
			.MaxWidth(96)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Password"))
			]

			+ SHorizontalBox::Slot()
			.MaxWidth(128)
			//.Padding(4)
			[
				SNew(SEditableTextBox)
				.IsPassword(true)
				.MinDesiredWidth(128)
				.Visibility(this, &FCognitiveTools::LoginTextboxUsable)
				.OnTextChanged(this,&FCognitiveTools::OnPasswordChanged)
			]

			+ SHorizontalBox::Slot() //log in
			.MaxWidth(128)
				.Padding(4, 0, 0, 0)
			[
				SNew(SButton)
				.Visibility(this, &FCognitiveTools::GetLoginButtonState)
				.IsEnabled(this, &FCognitiveTools::HasValidLogInFields)
				.Text(FText::FromString("Log In"))
				.OnClicked(this, &FCognitiveTools::LogIn)
			]
			
			+ SHorizontalBox::Slot() //log out
			.MaxWidth(128)
			.Padding(4,0,0,0)
			[
				SNew(SButton)
				.Visibility(this, &FCognitiveTools::GetLogoutButtonState)
				.IsEnabled(true)
				.Text(FText::FromString("Log Out"))
				.OnClicked(this, &FCognitiveTools::LogOut)
			]
		];

	//simple spacer
	LoginCategory.AddCustomRow(FText::FromString("Commands"))
	.ValueContent()
	.HAlign(HAlign_Fill)
	[
		SNew(SHorizontalBox)
		.Visibility(EVisibility::Hidden)
	];

	//ORGANIZATION DROPDOWN
	LoginCategory.AddCustomRow(FText::FromString("Commands"))
	.ValueContent()
	.HAlign(HAlign_Fill)
	[
		SNew(SHorizontalBox)
		//button
		+ SHorizontalBox::Slot()
		.MaxWidth(96)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Organization"))
		]

		+ SHorizontalBox::Slot()
		.MaxWidth(128)
		//.Padding(4)
		[
			SNew(STextComboBox)
			.OptionsSource(&AllOrgNames)
			.IsEnabled(this, &FCognitiveTools::HasLoggedIn)				
			.OnSelectionChanged(this, &FCognitiveTools::OnOrganizationChanged)
		]
	];

	//PRODUCT DROPDOWN
	LoginCategory.AddCustomRow(FText::FromString("Commands"))
	.ValueContent()
	.HAlign(HAlign_Fill)
	[
		SNew(SHorizontalBox)
		//button
		+ SHorizontalBox::Slot()
		.MaxWidth(96)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Product"))
		]

		+ SHorizontalBox::Slot()
		.MaxWidth(128)
		//.Padding(4)
		[
			SNew(STextComboBox)
			.IsEnabled(this,&FCognitiveTools::HasLoggedIn)
			.OptionsSource(&AllProductNames)
			//.ToolTip(SNew(SToolTip).Text(LOCTEXT("BaseColorFBXImportToolTip", "this is a tooltip")))
			.OnSelectionChanged(this, &FCognitiveTools::OnProductChanged)
			//.InitiallySelectedItem(GetProductNameFromFile())
		]
	];

	LoginCategory.AddCustomRow(FText::FromString("Commands"))
	.ValueContent()
	.MinDesiredWidth(256)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.MaxWidth(128)
		[
			SNew(SCheckBox)
			//.Style(FCoreStyle::Get(), "RadioButton")
			.IsEnabled(this, &FCognitiveTools::HasLoadedOrSelectedValidProduct)
			.IsChecked(this, &FCognitiveTools::HandleRadioButtonIsChecked, EReleaseType::Test)
			.OnCheckStateChanged(this, &FCognitiveTools::HandleRadioButtonCheckStateChanged, EReleaseType::Test)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Test"))
			]
		]
	
		+ SHorizontalBox::Slot()
		.MaxWidth(128)
		[
			SNew(SCheckBox)
			//.Style(FCoreStyle::Get(), "RadioButton")
			.IsEnabled(this, &FCognitiveTools::HasLoadedOrSelectedValidProduct)
			.IsChecked(this, &FCognitiveTools::HandleRadioButtonIsChecked, EReleaseType::Production)
			.OnCheckStateChanged(this, &FCognitiveTools::HandleRadioButtonCheckStateChanged, EReleaseType::Production)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Production"))
			]
		]
	];

	//simple spacer
	LoginCategory.AddCustomRow(FText::FromString("Commands"))
	.ValueContent()
	.HAlign(HAlign_Fill)
	[
		SNew(SHorizontalBox)
		.Visibility(EVisibility::Hidden)
	];

	LoginCategory.AddCustomRow(FText::FromString("Commands"))
	.ValueContent()
	.MinDesiredWidth(256)
	[
		SNew(STextBlock)
		.Text(FText::FromString(FCognitiveTools::GetCustomerIdFromFile()))
	];

	LoginCategory.AddCustomRow(FText::FromString("Commands"))
	.ValueContent()
	.MinDesiredWidth(256)
	[
		SNew(SButton)
		.Text(FText::FromString("Save"))
		.IsEnabled(this,&FCognitiveTools::HasSelectedValidProduct)
		.OnClicked(this, &FCognitiveTools::SaveCustomerIdToFile)
	];

	//refresh doens't usually reload anything. unclear how unreal can reload ini files

	LoginCategory.AddCustomRow(FText::FromString("Commands"))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(STextBlock)
				.ColorAndOpacity(FLinearColor::Yellow)
				.Text(FText::FromString("You must restart Unreal Editor to see changes in your config files here!"))
			]
		];

	LoginCategory.AddCustomRow(FText::FromString("Commands"))
	//.ValueContent()
	//.MinDesiredWidth(896)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		[
			SNew(SListView<TSharedPtr<FEditorSceneData>>)
			.ItemHeight(24)
			.ListItemsSource(&SceneData)
			.OnGenerateRow(this, &FCognitiveTools::OnGenerateWorkspaceRow)
			.HeaderRow(
				SNew(SHeaderRow)
				+ SHeaderRow::Column("name")
				.FillWidth(1)
				[
					SNew(STextBlock)
					//.MinDesiredWidth(256)
					.Text(FText::FromString("Name"))
				]

				+ SHeaderRow::Column("id")
				.FillWidth(1)
				[
					SNew(STextBlock)
					//.MinDesiredWidth(512)
					.Text(FText::FromString("Id"))
				]

				+ SHeaderRow::Column("version number")
				.FillWidth(0.3)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Version Number"))
				]

				+ SHeaderRow::Column("version id")
				.FillWidth(0.3)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Version Id"))
				]

				+ SHeaderRow::Column("open")
				[
					SNew(STextBlock)
					.Text(FText::FromString("Scene Explorer"))
				]
			)
		]
	];

	LoginCategory.AddCustomRow(FText::FromString("Commands"))
	.ValueContent()
	.MinDesiredWidth(256)
	[
		SNew(SButton)
		.IsEnabled(true)
		.Text(FText::FromString("Get Latest Scene Version Data"))
		.ToolTip(SNew(SToolTip).Text(LOCTEXT("get scene data tip", "This will get the latest scene version data from Scene Explorer and saves it to your config files. Must restart Unreal Editor to see the changes in your config files here")))
		.OnClicked(this, &FCognitiveTools::DebugRefreshCurrentScene)
	];


	IDetailCategoryBuilder& SceneWorkflow = DetailBuilder.EditCategory(TEXT("Scene Upload Workflow"));

	SceneWorkflow.AddCustomRow(FText::FromString("Commands"))
	//.ValueContent()
	//.MinDesiredWidth(896)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(4.0f, 0.0f)
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.Padding(0,0,0,4)
			.HAlign(EHorizontalAlignment::HAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Export Settings"))
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SButton)
				.IsEnabled(true)
				.Text(FText::FromString("Select Blender.exe"))
				.OnClicked(this, &FCognitiveTools::Select_Blender)
			]
			//TODO just a checkmark if blender path ends in blender.exe
			/*+SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Text(this, &FCognitiveTools::GetBlenderPath)
			]*/
			+ SVerticalBox::Slot()
			[
				SNew(SProperty, ExcludeMeshProperty)
				.IsEnabled(this, &FCognitiveTools::HasFoundBlender)
				.ShouldDisplayName(true)
			]
			+SVerticalBox::Slot()
			[
				SNew(SProperty,StaticOnlyProperty)
				.IsEnabled(this, &FCognitiveTools::HasFoundBlender)
				.ShouldDisplayName(true)
			]
			+ SVerticalBox::Slot()
			[
				SNew(SProperty, MinSizeProperty)
				.IsEnabled(this, &FCognitiveTools::HasFoundBlender)
				.ShouldDisplayName(true)
			]
			+ SVerticalBox::Slot()
			[
				SNew(SProperty, MaxSizeProperty)
				.IsEnabled(this, &FCognitiveTools::HasFoundBlender)
				.ShouldDisplayName(true)
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SButton)
				.IsEnabled(this, &FCognitiveTools::HasFoundBlender)
				.Text(FText::FromString("Select Export Meshes"))
				.OnClicked(this, &FCognitiveTools::Select_Export_Meshes)
			]
		]

		+ SHorizontalBox::Slot()
		.Padding(4.0f, 0.0f)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()

			.HAlign(EHorizontalAlignment::HAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Export"))
			]

			+ SVerticalBox::Slot()
			.Padding(0, 0, 0, 4)
			.HAlign(EHorizontalAlignment::HAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Important - select export as \"*.obj\"!"))
			]

			+ SVerticalBox::Slot()
			.MaxHeight(32)
			[
				SNew(SButton)
				.IsEnabled(this, &FCognitiveTools::HasFoundBlenderAndHasSelection)
				.Text(FText::FromString("Export Selected Scene Actors"))
				.OnClicked(this, &FCognitiveTools::Export_Selected)
			]
			+ SVerticalBox::Slot()
			.MaxHeight(32)
			[
				SNew(SButton)
				.IsEnabled(this, &FCognitiveTools::HasFoundBlender)
				.Text(FText::FromString("Export All Scene Actors"))
				.OnClicked(this, &FCognitiveTools::Export_All)
			]
		]
		+ SHorizontalBox::Slot()
		.Padding(4.0f, 0.0f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(0, 0, 0, 4)
			.HAlign(EHorizontalAlignment::HAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Optimize Files"))
			]

			+SVerticalBox::Slot()
			[
				SNew(SButton)
				.IsEnabled(this, &FCognitiveTools::HasFoundBlender)
				.Text(FText::FromString("Select Export Directory"))
				.OnClicked(this, &FCognitiveTools::Select_Export_Directory)
			]
			+ SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Text(this, &FCognitiveTools::GetExportDirectory)
			]

			+ SVerticalBox::Slot()
			[
				SNew(SButton)
				.IsEnabled(this, &FCognitiveTools::HasSetExportDirectory)
				.Text(FText::FromString("Export Transparent Textures"))
				.OnClicked(this, &FCognitiveTools::List_Materials)
			]


			+ SVerticalBox::Slot()
			[
				SNew(SProperty, MinPolygonProperty)
				.IsEnabled(this, &FCognitiveTools::HasFoundBlenderAndExportDir)
				.ShouldDisplayName(true)
			]
			+ SVerticalBox::Slot()
			[
				SNew(SProperty, MaxPolygonProperty)
				.IsEnabled(this, &FCognitiveTools::HasFoundBlenderAndExportDir)
				.ShouldDisplayName(true)
			]
			+ SVerticalBox::Slot()
			[
				SNew(SButton)
				.IsEnabled(this, &FCognitiveTools::HasFoundBlenderAndExportDir)
				.Text(FText::FromString("Reduce Mesh Topology"))
				.OnClicked(this, &FCognitiveTools::Reduce_Meshes)
			]

			+ SVerticalBox::Slot()
			[
				SNew(SProperty, TextureResizeProperty)
				.IsEnabled(this, &FCognitiveTools::HasFoundBlenderAndExportDir)
				.ShouldDisplayName(true)
			]
			+ SVerticalBox::Slot()
			[
				SNew(SButton)
				.IsEnabled(this, &FCognitiveTools::HasFoundBlenderAndExportDir)
				.Text(FText::FromString("Convert Textures"))
				.OnClicked(this, &FCognitiveTools::Reduce_Textures)
			]
		]
		+ SHorizontalBox::Slot()
		.Padding(4.0f, 0.0f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(0, 0, 0, 4)
			.HAlign(EHorizontalAlignment::HAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Upload"))
			]
			+ SVerticalBox::Slot()
			.MaxHeight(32)
			[
				SNew(SButton)
				.IsEnabled(this, &FCognitiveTools::HasConvertedFilesInDirectory)
				.Text(FText::FromString("Upload Scene"))
				//.ToolTip(TEXT("Make sure you have settings.json and no .bmp files in your export directory"))
				.ToolTip(SNew(SToolTip).Text(LOCTEXT("export tip", "Make sure you have settings.json and no .bmp files in your export directory")))
				.OnClicked(this, &FCognitiveTools::UploadScene)
			]
		]
		+ SHorizontalBox::Slot()
		.Padding(4.0f, 0.0f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(0, 0, 0, 4)
			.HAlign(EHorizontalAlignment::HAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Scene Explorer"))
			]
			+ SVerticalBox::Slot()
			.MaxHeight(32)
			[
				SNew(SButton)
				.IsEnabled(this, &FCognitiveTools::CurrentSceneHasSceneId)
				.Text(FText::FromString("Open Current Scene in Browser..."))
				.OnClicked(this, &FCognitiveTools::OpenCurrentSceneInBrowser)
			]
		]
	];


	IDetailCategoryBuilder& DynamicWorkflow = DetailBuilder.EditCategory(TEXT("Dynamic Upload Workflow"));

	DynamicWorkflow.AddCustomRow(FText::FromString("Commands"))
		//.ValueContent()
		//.MinDesiredWidth(896)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(4.0f, 0.0f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(0, 0, 0, 4)
				.HAlign(EHorizontalAlignment::HAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Scene Settings"))
				]
				+ SVerticalBox::Slot()
				[
					SNew(STextBlock)
					.Text(FText::FromString("this many dynamics: 0"))
				]
				+ SVerticalBox::Slot()
				[
					SNew(STextBlock)
					.Text(FText::FromString("warning some duplicate ids"))
				]

				+ SVerticalBox::Slot()
				[
					SNew(SButton)
					.IsEnabled(true)
					.Text(FText::FromString("Set Unique Dynamic Ids"))
					.OnClicked(this, &FCognitiveTools::SetUniqueDynamicIds)
				]
			]
			+ SHorizontalBox::Slot()
			.Padding(4.0f, 0.0f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(0, 0, 0, 4)
				.HAlign(EHorizontalAlignment::HAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Export"))
				]
				+ SVerticalBox::Slot()
				[
					SNew(SButton)
					.IsEnabled(this, &FCognitiveTools::HasFoundBlender)
					.Text(FText::FromString("Export Selected Dynamic Objects"))
					.OnClicked(this, &FCognitiveTools::ExportSelectedDynamics)
				]
				+ SVerticalBox::Slot()
				[
					SNew(SButton)
					.IsEnabled(this, &FCognitiveTools::HasFoundBlender)
					.Text(FText::FromString("Export All Dynamic Objects"))
					.OnClicked(this, &FCognitiveTools::ExportDynamics)
				]
			]
			+ SHorizontalBox::Slot()
			.Padding(4.0f, 0.0f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(0, 0, 0, 4)
				.HAlign(EHorizontalAlignment::HAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Organize Meshes"))
				]
				+ SVerticalBox::Slot()
				[
					SNew(SButton)
					.IsEnabled(true)
					.Text(FText::FromString("Select Dynamic Directory"))
					.OnClicked(this, &FCognitiveTools::SelectDynamicsDirectory)
				]
				+ SVerticalBox::Slot()
				[
					SNew(STextBlock)
					.Text(this, &FCognitiveTools::GetDynamicExportDirectory)
				]
			]
			+ SHorizontalBox::Slot()
			.Padding(4.0f, 0.0f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(0, 0, 0, 4)
				.HAlign(EHorizontalAlignment::HAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Upload"))
				]
				+ SVerticalBox::Slot()
				[
					SNew(SButton)
					.IsEnabled(this, &FCognitiveTools::HasSetDynamicExportDirectory)
					.Text(FText::FromString("Upload Dynamic Objects"))
					.OnClicked(this, &FCognitiveTools::UploadDynamics)
				]
			]
			+ SHorizontalBox::Slot()
			.Padding(4.0f, 0.0f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(0, 0, 0, 4)
				.HAlign(EHorizontalAlignment::HAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Scene Explorer"))
				]
				+ SVerticalBox::Slot()
				[
					SNew(STextBlock)
					.Text(FText::FromString("list of dynamic objects on SE"))
				]
				+ SVerticalBox::Slot()
				[
					SNew(STextBlock)
					.Text(FText::FromString("total number of objects on SE"))
				]
				+ SVerticalBox::Slot()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					[
						SNew(SButton)
						.IsEnabled(true)
						.Text(FText::FromString("Get Dynamics from SE"))
						.OnClicked(this, &FCognitiveTools::GetDynamicsManifest)
					]
					+ SHorizontalBox::Slot()
					[
						SNew(SButton)
						.IsEnabled(true)
						.Text(FText::FromString("Post Dynamics to SE"))
						.OnClicked(this, &FCognitiveTools::UploadDynamicsManifest)
					]
					/*+ SHorizontalBox::Slot()
					[
						SNew(SImage)
						.Image(FCoreStyle::Get().GetBrush("Icons.Warning"))
					]*/
				]
			]
		];

	FCognitiveTools::RefreshSceneData();
}

TSharedRef<ITableRow> FCognitiveTools::OnGenerateWorkspaceRow(TSharedPtr<FEditorSceneData> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return
		SNew(SComboRow< TSharedPtr<FEditorSceneData> >, OwnerTable)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1)
			.Padding(2.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(InItem->Name))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1)
			.Padding(2.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(InItem->Id))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(0.3)
			.Padding(2.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(FString::FromInt(InItem->VersionNumber)))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(0.3)
			.Padding(2.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(FString::FromInt(InItem->VersionId)))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1)
			.Padding(2.0f)
			[
				SNew(SButton)
				.Text(FText::FromString("Open in Browser..."))
				.OnClicked(this,&FCognitiveTools::OpenSceneInBrowser,InItem->Id)
			]
		];
}

bool FCognitiveTools::HasValidLogInFields() const
{
	return Email.Len() > 0 && Password.Len() > 0;
}

EVisibility FCognitiveTools::LoginTextboxUsable() const
{
	if (HasLoggedIn())
		return EVisibility::HitTestInvisible;
	return EVisibility::Visible;
}

FText FCognitiveTools::GetDynamicsFromManifest() const
{
	return FText::FromString("DYNAMICS");
}

FReply FCognitiveTools::LogOut()
{
	Email = "";
	//OnEmailChanged(TEXT(""));
	Password = "";
	//OnPasswordChanged(TEXT(""));

	FAnalyticsCognitiveVR::Get().EditorAuthToken = "";
	FAnalyticsCognitiveVR::Get().EditorSessionId = "";
	FAnalyticsCognitiveVR::Get().EditorSessionToken = "";

	return FReply::Handled();
}

FReply FCognitiveTools::DebugRefreshCurrentScene()
{
	TSharedPtr<FEditorSceneData> scenedata = GetCurrentSceneData();

	if (scenedata.IsValid())
	{
		SceneVersionRequest(*scenedata);
	}

	return FReply::Handled();
}

EVisibility FCognitiveTools::GetLogoutButtonState() const
{
	if (HasLoggedIn())
		return EVisibility::Visible;
	return EVisibility::Collapsed;
}

EVisibility FCognitiveTools::GetLoginButtonState() const
{
	if (HasLoggedIn())
		return EVisibility::Collapsed;
	return EVisibility::Visible;
}

FString FCognitiveTools::GetCustomerIdFromFile() const
{
	FString customerid;
	GConfig->GetString(TEXT("Analytics"), TEXT("CognitiveVRApiKey"), customerid, GEngineIni);
	return customerid;
}

bool FCognitiveTools::HasSavedCustomerId() const
{
	FString customerid = GetCustomerIdFromFile();
	if (customerid.Len() == 0) { return false; }
	if (customerid.EndsWith("-test")) { return true; }
	if (customerid.EndsWith("-prod")) { return true; }
	return false;
}

void FCognitiveTools::SaveOrganizationNameToFile(FString organization)
{
	//GConfig->SetString(TEXT("Analytics"), TEXT("CognitiveOrganization"), *organization, GEngineIni);
	//GConfig->Flush(false, GEngineIni);
}

TSharedPtr< FString > FCognitiveTools::GetOrganizationNameFromFile()
{
	FString organization;
	GConfig->GetString(TEXT("Analytics"), TEXT("CognitiveOrganization"), organization, GEngineIni);
	return MakeShareable(new FString(organization));
}

void FCognitiveTools::SaveProductNameToFile(FString product)
{
	//GConfig->SetString(TEXT("Analytics"), TEXT("CognitiveProduct"), *product, GEngineIni);
	//GConfig->Flush(false, GEngineIni);
}

TSharedPtr< FString > FCognitiveTools::GetProductNameFromFile()
{
	FString product;
	GConfig->GetString(TEXT("Analytics"), TEXT("CognitiveProduct"), product, GEngineIni);
	return MakeShareable(new FString(product));
}

FReply FCognitiveTools::OpenSceneInBrowser(FString sceneid)
{
	FString url = SceneExplorerOpen(sceneid);

	FPlatformProcess::LaunchURL(*url, nullptr, nullptr);

	return FReply::Handled();
}

FReply FCognitiveTools::OpenCurrentSceneInBrowser()
{
	TSharedPtr<FEditorSceneData> scenedata = GetCurrentSceneData();

	if (!scenedata.IsValid())
	{
		return FReply::Handled();
	}

	FString url = SceneExplorerOpen(scenedata->Id);

	return FReply::Handled();
}

// Callback for checking a radio button.
void FCognitiveTools::HandleRadioButtonCheckStateChanged(ECheckBoxState NewRadioState, EReleaseType RadioThatChanged)
{
	if (NewRadioState == ECheckBoxState::Checked)
	{
		RadioChoice = RadioThatChanged;
	}

	if (HasSelectedValidProduct())
	{
		SaveCustomerIdToFile();
	}
}

FReply FCognitiveTools::RefreshSceneData()
{
	SceneData.Empty();

	//GConfig->UnloadFile(GEngineIni);
	//GConfig->LoadFile(GEngineIni);

	TArray<FString>scenstrings;
	GConfig->GetArray(TEXT("/Script/CognitiveVR.CognitiveVRSceneSettings"), TEXT("SceneData"), scenstrings,GEngineIni);
	
	GLog->Log("GEngineIni is " + GEngineIni);

	for (int i = 0; i < scenstrings.Num(); i++)
	{
		TArray<FString> Array;
		scenstrings[i].ParseIntoArray(Array, TEXT(","), true);

		if (Array.Num() == 2) //scenename,sceneid
		{
			//old scene data. append versionnumber and versionid
			Array.Add("1");
			Array.Add("0");
		}

		if (Array.Num() != 4)
		{
			GLog->Log("failed to parse " + scenstrings[i]);
			continue;
		}

		FEditorSceneData* tempscene = new FEditorSceneData(Array[0], Array[1], FCString::Atoi(*Array[2]), FCString::Atoi(*Array[3]));
		SceneData.Add(MakeShareable(tempscene));
	}
	
	GLog->Log("FCognitiveToolsCustomization::RefreshSceneData found this many scenes: " + FString::FromInt(SceneData.Num()));

	return FReply::Handled();
}

void FCognitiveTools::SceneVersionRequest(FEditorSceneData data)
{
	if (FAnalyticsCognitiveVR::Get().EditorAuthToken.Len() == 0)
	{
		GLog->Log("FCognitiveTools::SceneVersionRequest no auth token. TODO get auth token and retry");
		return;
	}

	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->SetURL(GetSceneVersion(data.Id));

	GLog->Log("url "+GetSceneVersion(data.Id));
	GLog->Log("auth token " + FAnalyticsCognitiveVR::Get().EditorAuthToken);

	HttpRequest->SetHeader("X-HTTP-Method-Override", TEXT("GET"));
	HttpRequest->SetHeader("Authorization", TEXT("Bearer " + FAnalyticsCognitiveVR::Get().EditorAuthToken));

	HttpRequest->OnProcessRequestComplete().BindSP(this, &FCognitiveTools::SceneVersionResponse);
	HttpRequest->ProcessRequest();
}

void FCognitiveTools::SceneVersionResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	int32 responseCode = Response->GetResponseCode();

	GLog->Log("FCognitiveToolsCustomization::GetSceneVersionResponse Code: " + FString::FromInt(responseCode));

	GLog->Log(Response->GetContentAsString());

	if (responseCode >= 500)
	{
		//internal server error
		GLog->Log("FCognitiveToolsCustomization::SceneVersionResponse 500-ish internal server error");
		return;
	}
	if (responseCode >= 400)
	{
		if (responseCode == 401)
		{
			//not authorized
			GLog->Log("FCognitiveToolsCustomization::SceneVersionResponse not authorized!");
			return;
		}
		else
		{
			//maybe no scene?
			GLog->Log("FCognitiveToolsCustomization::SceneVersionResponse some error. maybe no scene?");
			return;
		}
	}

	//parse response content to json

	TSharedPtr<FJsonObject> JsonSceneSettings;

	TSharedRef<TJsonReader<>>Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(Reader, JsonSceneSettings))
	{
		//get the latest version of the scene
		int32 versionNumber = 0;
		int32 versionId = 0;
		TArray<TSharedPtr<FJsonValue>> versions = JsonSceneSettings->GetArrayField("versions");
		for (int i = 0; i < versions.Num(); i++) {

			int32 tempversion = versions[i]->AsObject()->GetNumberField("versionnumber");
			if (tempversion > versionNumber)
			{
				versionNumber = tempversion;
				versionId = versions[i]->AsObject()->GetNumberField("id");
			}
		}
		if (versionNumber + versionId == 0)
		{
			GLog->Log("FCognitiveToolsCustomization::SceneVersionResponse couldn't find a latest version in SceneVersion data");
			return;
		}

		//check that there is scene data in ini
		TSharedPtr<FEditorSceneData> currentSceneData = GetCurrentSceneData();
		if (!currentSceneData.IsValid())
		{
			GLog->Log("FCognitiveToolsCustomization::SceneVersionResponse can't find current scene data in ini files");
			return;
		}

		//get array of scene data
		TArray<FString> iniscenedata;

		GConfig->GetArray(TEXT("/Script/CognitiveVR.CognitiveVRSceneSettings"), TEXT("SceneData"), iniscenedata, GEngineIni);

		GLog->Log("found this many scene datas in ini " + FString::FromInt(iniscenedata.Num()));
		GLog->Log("looking for scene " + currentSceneData->Name);

		//update current scene
		for (int i = 0; i < iniscenedata.Num(); i++)
		{
			GLog->Log("looking at data " + iniscenedata[i]);

			TArray<FString> entryarray;
			iniscenedata[i].ParseIntoArray(entryarray, TEXT(","), true);

			if (entryarray[0] == currentSceneData->Name)
			{
				iniscenedata[i] = entryarray[0] + "," + entryarray[1] + "," + FString::FromInt(versionNumber) + "," + FString::FromInt(versionId);
				GLog->Log("FCognitiveToolsCustomization::SceneVersionResponse overwriting scene data to append versionnumber and versionid");
				GLog->Log(iniscenedata[i]);
				break;
			}
			else
			{
				//GLog->Log("found scene " + entryarray[0]);
			}
		}
		//set array to config
		GConfig->RemoveKey(TEXT("/Script/CognitiveVR.CognitiveVRSceneSettings"), TEXT("SceneData"), GEngineIni);
		//GConfig->Remove(
		GConfig->SetArray(TEXT("/Script/CognitiveVR.CognitiveVRSceneSettings"), TEXT("SceneData"), iniscenedata, GEngineIni);
		GConfig->Flush(false, GEngineIni);
	}
	else
	{
		GLog->Log("FCognitiveToolsCustomization::SceneVersionResponse failed to parse json response");
	}
}

TArray<TSharedPtr<FEditorSceneData>> FCognitiveTools::GetSceneData() const
{
	return SceneData;
}

// Callback for determining whether a radio button is checked.
ECheckBoxState FCognitiveTools::HandleRadioButtonIsChecked(EReleaseType ButtonId) const
{
	return (RadioChoice == ButtonId)
		? ECheckBoxState::Checked
		: ECheckBoxState::Unchecked;
}

bool FCognitiveTools::HasLoggedIn() const
{
	return FAnalyticsCognitiveVR::Get().EditorSessionToken.Len() > 0;
}

bool FCognitiveTools::HasSelectedValidProduct() const
{
	return SelectedProduct.customerId.Len() > 0;
}

bool FCognitiveTools::HasLoadedOrSelectedValidProduct() const
{
	if (SelectedProduct.customerId.Len() > 0) { return true; }
	if (HasSavedCustomerId()) { return true; }
	return false;
}

FCognitiveTools::EReleaseType FCognitiveTools::GetReleaseTypeFromFile()
{
	FString customerid = FCognitiveTools::GetCustomerIdFromFile();

	if (customerid.Len() > 0)
	{
		if (customerid.EndsWith("-prod"))
		{
			return EReleaseType::Production;
		}
	}
	return EReleaseType::Test;
}

//TODO load releasetype and selected customer+product from ini file

FReply FCognitiveTools::SaveCustomerIdToFile()
{
	FString CustomerId = SelectedProduct.customerId;

	if (RadioChoice == EReleaseType::Test)
	{
		CustomerId.Append("-test");
	}
	else
	{
		CustomerId.Append("-prod");
	}
	
	GLog->Log("write customer id to ini: " + CustomerId);
	GConfig->SetString(TEXT("Analytics"), TEXT("ProviderModuleName"), TEXT("CognitiveVR"), GEngineIni);
	GConfig->SetString(TEXT("Analytics"), TEXT("CognitiveVRApiKey"), *CustomerId, GEngineIni);

	SaveProductNameToFile(SelectedProduct.name);

	GConfig->Flush(false, GEngineIni);

	return FReply::Handled();
}

void FCognitiveTools::OnEmailChanged(const FText& Text)
{
	Email = Text.ToString();
}

void FCognitiveTools::OnPasswordChanged(const FText& Text)
{
	Password = Text.ToString();
}

void FCognitiveTools::OnProductChanged(TSharedPtr<FString> Selection, ESelectInfo::Type SelectInfo)
{
	if (!Selection.IsValid()) { return; }

	FString newProductName = *Selection;
	for (int i = 0; i < ProductInfos.Num(); i++)
	{
		if (newProductName == ProductInfos[i].name)
		{
			SelectedProduct = ProductInfos[i];
			return;
		}
	}
}

void FCognitiveTools::OnOrganizationChanged(TSharedPtr<FString> Selection, ESelectInfo::Type SelectInfo)
{
	if (!Selection.IsValid()) { return; }

	FString newOrgName = *Selection;

	GLog->Log("organization selection changed " + newOrgName);

	FOrganizationData selectedOrg;

	for (int i = 0; i < OrganizationInfos.Num(); i++)
	{
		if (OrganizationInfos[i].name == newOrgName)
		{
			selectedOrg = OrganizationInfos[i];
			break;
		}
	}

	SaveOrganizationNameToFile(selectedOrg.name);

	ProductInfos.Empty();

	TArray<TSharedPtr<FJsonValue>> mainArray = JsonUserData->GetArrayField("products");
	for (int RowNum = 0; RowNum != mainArray.Num(); RowNum++) {
		TSharedPtr<FJsonObject> tempRow = mainArray[RowNum]->AsObject();
		if (tempRow->GetStringField("orgId") != selectedOrg.id)
		{
			continue;
		}
		FProductData tempProduct;
		tempProduct.id = tempRow->GetStringField("id");
		tempProduct.name = tempRow->GetStringField("name");
		tempProduct.orgId = tempRow->GetStringField("orgId");
		tempProduct.customerId = tempRow->GetStringField("customerId");
		ProductInfos.Add(tempProduct);

		AllProductNames.Add(MakeShareable(new FString(tempProduct.name)));
	}
}

TArray<TSharedPtr<FString>> FCognitiveTools::GetOrganizationNames()
{
	return AllOrgNames;
}

FReply FCognitiveTools::PrintSessionId()
{
	FString editorSessionId = FAnalyticsCognitiveVR::Get().EditorSessionId;
	GLog->Log(editorSessionId);
	return FReply::Handled();
}

FReply FCognitiveTools::LogIn()
{
	//how to send request and listen for response?

	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->SetVerb("POST");
	HttpRequest->SetURL("https://api.cognitivevr.io/sessions");

	FString body = "{\"email\":\"" + Email + "\",\"password\":\"" + Password + "\"}";

	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));

	HttpRequest->SetContentAsString(body);

	HttpRequest->OnProcessRequestComplete().BindSP(this, &FCognitiveTools::OnLogInResponse);

	GLog->Log("send login request!");
	GLog->Log(FString::FromInt(HttpRequest->GetContentLength()));

	if (Email.Len() == 0)
	{
		GLog->Log("email length is 0");
		return FReply::Handled();
	}
	if (Password.Len() == 0)
	{
		GLog->Log("password length is 0");
		return FReply::Handled();
	}

	GLog->Log("email and password length > 0");

	HttpRequest->ProcessRequest();
	return FReply::Handled();
}

void FCognitiveTools::OnLogInResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (Response.IsValid())
	{
		GLog->Log("Login Response "+Response->GetContentAsString());
		GLog->Log("Login error code"+FString::FromInt(Response->GetResponseCode()));
		if (Response->GetResponseCode() == 201)
		{
			FAnalyticsCognitiveVR::Get().EditorSessionToken = Response->GetHeader("Set-Cookie");
			//request auth token
			//AuthTokenRequest();


			TArray<FString> Array;
			FString MyString(Response->GetHeader("Set-Cookie"));
			MyString.ParseIntoArray(Array, TEXT(";"), true);

			FAnalyticsCognitiveVR::Get().EditorSessionId = Array[0].RightChop(18);
			GLog->Log("token " + FAnalyticsCognitiveVR::Get().EditorSessionToken);
			GLog->Log("id " + FAnalyticsCognitiveVR::Get().EditorSessionId);
			
			//parse login response to userdata
			//read organization names from that
			//OrganizationNames =
			
			TSharedRef<TJsonReader<>>Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, JsonUserData))
			{
				TArray<TSharedPtr<FJsonValue>> mainArray = JsonUserData->GetArrayField("organizations");
				for (int RowNum = 0; RowNum != mainArray.Num(); RowNum++) {
					FOrganizationData tempOrg;
					TSharedPtr<FJsonObject> tempRow = mainArray[RowNum]->AsObject();
					tempOrg.id = tempRow->GetStringField("id");
					tempOrg.name = tempRow->GetStringField("name");
					tempOrg.prefix = tempRow->GetStringField("prefix");
					OrganizationInfos.Add(tempOrg);

					AllOrgNames.Add(MakeShareable(new FString(tempOrg.name)));
				}

				GLog->Log("found this many organizations: "+FString::FromInt(OrganizationInfos.Num()));

				AuthTokenRequest();
			}
		}
	}
	else
	{
		GLog->Log("Login Response is null");
	}
}

FReply FCognitiveTools::DEBUG_RequestAuthToken()
{
	AuthTokenRequest();
	return FReply::Handled();
}

void FCognitiveTools::AuthTokenRequest()
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	TSharedPtr<FEditorSceneData> currentscenedata = GetCurrentSceneData();
	if (!currentscenedata.IsValid())
	{
		GLog->Log("FCogntiveToolsCustomization::AuthTokenRequest cannot find current scene data");
		return;
	}
	if (FAnalyticsCognitiveVR::Get().EditorSessionToken.Len() == 0)
	{
		GLog->Log("FCogntiveTools::AuthTokenRequest session token is invalid. log in");
		return;
	}

	GLog->Log("FCognitiveToolsCustomization::AuthTokenRequest send auth token request");
	GLog->Log("url "+PostAuthToken(currentscenedata->Id));
	GLog->Log("cookie " + FAnalyticsCognitiveVR::Get().EditorSessionToken);

	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader("Cookie", FAnalyticsCognitiveVR::Get().EditorSessionToken);
	HttpRequest->SetURL(PostAuthToken(currentscenedata->Id));
	HttpRequest->OnProcessRequestComplete().BindSP(this, &FCognitiveTools::AuthTokenResponse);
	HttpRequest->ProcessRequest();
}

void FCognitiveTools::AuthTokenResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!Response.IsValid())
	{
		GLog->Log("FCognitiveToolsCustomization::AuthTokenResponse response isn't valid");
		return;
	}

	GLog->Log("FCognitiveToolsCustomization::AuthTokenResponse response code " + FString::FromInt(Response->GetResponseCode()));
	GLog->Log("FCognitiveToolsCustomization::AuthTokenResponse " + Response->GetContentAsString());

	if (bWasSuccessful)
	{
		TSharedPtr<FJsonObject> JsonAuthToken;
		TSharedRef<TJsonReader<>>Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonAuthToken))
		{
			FString token = JsonAuthToken->GetStringField("token");
			FAnalyticsCognitiveVR::Get().EditorAuthToken = token;
		}
	}
}

FReply FCognitiveTools::ReexportDynamicMeshesCmd()
{
	ReexportDynamicMeshes(ExportDynamicsDirectory);
	return FReply::Handled();
}

FReply FCognitiveTools::ExportDynamicTextures()
{
	ConvertDynamicTextures();
	return FReply::Handled();
}


TSharedPtr<FEditorSceneData> FCognitiveTools::GetCurrentSceneData() const
{
	UWorld* myworld = GWorld->GetWorld();

	FString currentSceneName = myworld->GetMapName();
	currentSceneName.RemoveFromStart(myworld->StreamingLevelsPrefix);
	return GetSceneData(currentSceneName);
}

TSharedPtr<FEditorSceneData> FCognitiveTools::GetSceneData(FString scenename) const
{
	for (int i = 0; i < SceneData.Num(); i++)
	{
		if (!SceneData[i].IsValid()) { continue; }
		if (SceneData[i]->Name == scenename)
		{
			return SceneData[i];
		}
	}
	GLog->Log("FCognitiveToolsCustomization::GetSceneData couldn't find SceneData for scene " + scenename);
	return NULL;
}

FReply FCognitiveTools::DebugSendSceneData()
{
	SaveSceneData("FirstPersonExampleMap1234", "1234-asdf-5678-hjkl");
	return FReply::Handled();
}

void FCognitiveTools::SaveSceneData(FString sceneName, FString sceneKey)
{
	FString keyValue = sceneName + "," + sceneKey;
	UE_LOG(LogTemp, Warning, TEXT("Upload complete! Add this into the SceneData array in Project Settings:      %s"),*keyValue);


	TArray<FString> scenePairs = TArray<FString>();

	GConfig->GetArray(TEXT("/Script/CognitiveVR.CognitiveVRSceneSettings"), TEXT("SceneData"), scenePairs, GEngineIni);

	bool didSetKey = false;
	for (int32 i = 0; i < scenePairs.Num(); i++)
	{
		FString name;
		FString key;
		scenePairs[i].Split(TEXT(","), &name, &key);
		if (*name == sceneName)
		{
			scenePairs[i] = keyValue;
			didSetKey = true;
			GLog->Log("FCognitiveToolsCustomization::SaveSceneData - found and replace key for scene " + name + " new value " + keyValue);
			break;
		}
	}
	if (!didSetKey)
	{
		scenePairs.Add(keyValue);
		GLog->Log("FCognitiveToolsCustomization::SaveSceneData - added new scene value and key for " + sceneName);
	}

	//remove scene names that don't have keys!
	for (int32 i = scenePairs.Num()-1; i >= 0; i--)
	{
		FString name;
		FString key;
		if (!scenePairs[i].Split(TEXT(","), &name, &key))
		{
			scenePairs.RemoveAt(i);
		}
	}

	GConfig->SetArray(TEXT("/Script/CognitiveVR.CognitiveVRSceneSettings"), TEXT("SceneData"), scenePairs, GEngineIni);

	GConfig->Flush(false, GEngineIni);
	//GConfig->UnloadFile(GEngineIni);
	//GConfig->LoadFile(GEngineIni);
}

#undef LOCTEXT_NAMESPACE