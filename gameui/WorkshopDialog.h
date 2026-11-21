//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Retail HL2 20th Anniversary Steam Workshop UI dialogs
//          Decompiled from retail GameUI.so.
//
//=============================================================================//

#ifndef WORKSHOPDIALOG_H
#define WORKSHOPDIALOG_H
#ifdef _WIN32
#pragma once
#endif

#include "vgui_controls/PropertyDialog.h"
#include "vgui_controls/PropertyPage.h"
#include "vgui_controls/PanelListPanel.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/Button.h"
#include "vgui_controls/CheckButton.h"
#include "vgui_controls/ComboBox.h"
#include "vgui_controls/TextEntry.h"
#include "vgui_controls/ProgressBar.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/Menu.h"
#include "vgui_controls/AnimationController.h"
#include "matchmaking/imatchevents.h"
#include "iworkshop.h"

class CWorkshopCampaignsPage;
class CWorkshopFeaturedPage;
class CWorkshopSubscribedPage;
class CWorkshopMyUploadsPage;
class CWorkshopUploaderPage;
class CWorkshopItemPanel;
class CSteamUGCUserItemsQuery;
class CWorkshopLocalBitmapImage;

//-----------------------------------------------------------------------------
// CWorkshopDialog
//-----------------------------------------------------------------------------
class CWorkshopDialog : public vgui::PropertyDialog, public IMatchEventsSink
{
	DECLARE_CLASS_SIMPLE( CWorkshopDialog, vgui::PropertyDialog );

public:
	CWorkshopDialog( vgui::Panel *parent );
	virtual ~CWorkshopDialog();

	virtual void OnCommand( const char *command ) OVERRIDE;
	virtual void OnKeyCodePressed( vgui::KeyCode code ) OVERRIDE;
	virtual void OnThink() OVERRIDE;
	virtual void PerformLayout() OVERRIDE;
	virtual void OnEvent( KeyValues *pEvent ) OVERRIDE;

private:
	void UpdateLayout();

	CWorkshopCampaignsPage  *m_pCampaignsPage;
	CWorkshopFeaturedPage   *m_pFeaturedPage;
	CWorkshopSubscribedPage *m_pSubscribedPage;
	CWorkshopMyUploadsPage  *m_pMyUploadsPage;

	vgui::Button            *m_pBrowseButton;
	vgui::Button            *m_pLoadButton;
	vgui::Button            *m_pUnusedButton;
	vgui::Button            *m_pEnableAllButton;
	vgui::Button            *m_pDisableAllButton;
	vgui::Button            *m_pNewItemButton;
	vgui::HFont              m_hFont;
};

//-----------------------------------------------------------------------------
// CWorkshopCampaignsPage
//-----------------------------------------------------------------------------
class CWorkshopCampaignsPage : public vgui::PropertyPage, public IMatchEventsSink
{
	DECLARE_CLASS_SIMPLE( CWorkshopCampaignsPage, vgui::PropertyPage );

public:
	CWorkshopCampaignsPage( vgui::Panel *parent, const char *name );
	virtual ~CWorkshopCampaignsPage();

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme ) OVERRIDE;
	virtual void PerformLayout() OVERRIDE;
	virtual void OnEvent( KeyValues *pEvent ) OVERRIDE;

private:
	void RefreshItems();

	CUtlVector<CWorkshopItemPanel *> m_CampaignPanels;
	int                              m_nItemWidth;
	int                              m_nItemHeight;
	vgui::PanelListPanel            *m_pList;
	CPanelAnimationVarAliasType( int, m_nGridSpacing, "grid_spacing", "8", "proportional_int" );
};

//-----------------------------------------------------------------------------
// CWorkshopSubscribedPage
//-----------------------------------------------------------------------------
class CWorkshopSubscribedPage : public vgui::PropertyPage, public IMatchEventsSink
{
	DECLARE_CLASS_SIMPLE( CWorkshopSubscribedPage, vgui::PropertyPage );

public:
	CWorkshopSubscribedPage( vgui::Panel *parent, const char *name );
	virtual ~CWorkshopSubscribedPage();

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme ) OVERRIDE;
	virtual void OnEvent( KeyValues *pEvent ) OVERRIDE;

private:
	void RefreshItems();

	vgui::PanelListPanel *m_pList;
};

//-----------------------------------------------------------------------------
// CWorkshopFeaturedPage (unused stub in retail)
//-----------------------------------------------------------------------------
class CWorkshopFeaturedPage : public vgui::PropertyPage
{
	DECLARE_CLASS_SIMPLE( CWorkshopFeaturedPage, vgui::PropertyPage );

public:
	CWorkshopFeaturedPage( vgui::Panel *parent, const char *name );
};

//-----------------------------------------------------------------------------
// CWorkshopItemPanel
//-----------------------------------------------------------------------------
class CWorkshopItemPanel : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CWorkshopItemPanel, vgui::EditablePanel );

public:
	CWorkshopItemPanel( vgui::Panel *parent, WorkshopItem_t *pItem,
	                    bool bListMode, bool bAllowReorder, bool bEditMode, int nIndex );
	virtual ~CWorkshopItemPanel();

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme ) OVERRIDE;
	virtual void OnCommand( const char *command ) OVERRIDE;
	virtual void OnMousePressed( vgui::MouseCode code ) OVERRIDE;
	virtual void OnMouseDoublePressed( vgui::MouseCode code ) OVERRIDE;
	virtual void OnThink() OVERRIDE;
	virtual void PerformLayout() OVERRIDE;

private:
	void UpdateDownloadState();

	WorkshopItem_t           *m_pItem;
	int                       m_nIndex;
	bool                      m_bAllowReorder;
	bool                      m_bEditMode;
	bool                      m_bListMode;
	uint32                    m_unItemState;
	vgui::DHANDLE<vgui::Menu> m_hActionMenu;
	IWorkshopImage           *m_pPreviewImage;
	vgui::ImagePanel         *m_pImagePanel;
	vgui::ImagePanel         *m_pTitleBacking;
	vgui::Label              *m_pTitleLabel;
	vgui::Label              *m_pLoadingLabel;
	vgui::Label              *m_pDescLabel;
	vgui::Button             *m_pPlayButton;
	vgui::Button             *m_pEditButton;
	vgui::Button             *m_pUpButton;
	vgui::Button             *m_pDownButton;
	vgui::CheckButton        *m_pCheckButton;
	vgui::IBorder            *m_pNormalBorder;
	vgui::IBorder            *m_pSelectedBorder;
	Color                     m_SelectionColor;
};

//-----------------------------------------------------------------------------
// CWorkshopMyUploadsPage
//-----------------------------------------------------------------------------
class CWorkshopMyUploadsPage : public vgui::PropertyPage, public IMatchEventsSink
{
	DECLARE_CLASS_SIMPLE( CWorkshopMyUploadsPage, vgui::PropertyPage );

public:
	CWorkshopMyUploadsPage( vgui::Panel *parent, const char *name );
	virtual ~CWorkshopMyUploadsPage();

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme ) OVERRIDE;
	virtual void OnEvent( KeyValues *pEvent ) OVERRIDE;

	void RefreshItems();

	vgui::PanelListPanel    *m_pList;
	CSteamUGCUserItemsQuery *m_pQuery;
};

//-----------------------------------------------------------------------------
// CWorkshopUploaderPage
//-----------------------------------------------------------------------------
class CWorkshopUploaderPage : public vgui::PropertyPage, public IMatchEventsSink
{
	DECLARE_CLASS_SIMPLE( CWorkshopUploaderPage, vgui::PropertyPage );

public:
	CWorkshopUploaderPage( vgui::Panel *parent, const char *name, WorkshopItem_t *pEditItem );
	virtual ~CWorkshopUploaderPage();

	virtual void OnTick() OVERRIDE;
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme ) OVERRIDE;
	virtual void OnCommand( const char *command ) OVERRIDE;
	virtual void OnEvent( KeyValues *pEvent ) OVERRIDE {}
	MESSAGE_FUNC_PTR( OnTextChanged, "TextChanged", panel );

	PublishedFileId_t GetPublishedFileId() const { return m_nPublishedFileId; }

private:
	void ClearPreviewImage();
	int  GetSelectedAddonType();
	void UpdateButtonStates();
	void UpdateTabTitle();
	void BringTabToFront();
	void OnUploadSucceeded( SubmitItemUpdateResult_t *pResult );
	void OnUploadFailed( const char *pszReason );
	void OnSubmitItemUpdate( SubmitItemUpdateResult_t *pResult, bool bIOFailure );
	void OnCreateItem( CreateItemResult_t *pResult, bool bIOFailure );
	void UpdateUploadParams();
	void StartUpload();

	WorkshopItem_t            *m_pInitialEditItem;
	vgui::ImagePanel          *m_pPreviewImagePanel;
	CWorkshopLocalBitmapImage *m_pLocalPreviewBitmap;
	vgui::TextEntry           *m_pTitleEntry;
	vgui::TextEntry           *m_pDescEntry;
	vgui::TextEntry           *m_pStartingMapEntry;
	vgui::Label               *m_pStartingMapLabel;
	vgui::Label               *m_pExpansionLabel;
	vgui::ComboBox            *m_pExpansionCombo;
	vgui::TextEntry           *m_pContentEntry;
	vgui::ComboBox            *m_pAddonTypeCombo;
	vgui::ComboBox            *m_pVisibilityCombo;
	vgui::Button              *m_pUploadButton;
	vgui::ProgressBar         *m_pProgressBar;

	CUtlString                 m_szPreviewImagePath;
	CUtlString                 m_szContentFolder;
	CUtlString                 m_szTitle;
	CUtlString                 m_szDescription;
	CUtlString                 m_szStartingMap;

	bool                       m_bUploading;
	bool                       m_bTitleDirty;
	bool                       m_bDescDirty;
	bool                       m_bStartingMapDirty;
	uint64                     m_ulTags[WORKSHOP_TAG_CATEGORY_COUNT];

	CCallResult<CWorkshopUploaderPage, CreateItemResult_t>       m_CallResultCreateItem;
	CCallResult<CWorkshopUploaderPage, SubmitItemUpdateResult_t> m_CallResultSubmitUpdate;

	PublishedFileId_t          m_nPublishedFileId;
	UGCUpdateHandle_t          m_hUpdateHandle;
};

#endif // WORKSHOPDIALOG_H
