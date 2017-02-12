#pragma once

#ifndef AUSERINTERFACE_H
#define AUSERINTERFACE_H

#include "Utils.h"
#include "boost/function.hpp"

#include <SDL.h>
#include <SDL_ttf.h>


using namespace std;


namespace UI
{
	enum EUserInterfaceFlags
	{
		UI_Tick = FLAGINDEX(0),
		UI_Render = FLAGINDEX(1),
		UI_Visible = FLAGINDEX(0) + FLAGINDEX(1),
		UI_Hidden = 0,

		UI_AutoSize = FLAGINDEX(2),
		UI_AutoAnchor = FLAGINDEX(3),
		UI_AutoArea = FLAGINDEX(2) + FLAGINDEX(3),

		UI_HorizontalBox = FLAGINDEX(4),
		UI_VerticalBox = FLAGINDEX(5),

		UI_Parented = FLAGINDEX(6)
	};


	enum EMouseEvent
	{
		Mouse_Nothing = FLAGINDEX(0),

		Mouse_Move = FLAGINDEX(1),

		Mouse_LClick = FLAGINDEX(2),
		Mouse_LHold = FLAGINDEX(3),
		Mouse_LDown = FLAGINDEX(2) + FLAGINDEX(3),
		Mouse_LUp = FLAGINDEX(4),

		Mouse_RClick = FLAGINDEX(5),
		Mouse_RHold = FLAGINDEX(6),
		Mouse_RDown = FLAGINDEX(5) + FLAGINDEX(6),
		Mouse_RUp = FLAGINDEX(7),

		Mouse_WDown = FLAGINDEX(8),
		Mouse_WUp = FLAGINDEX(9)
	};


	enum EButtonEvent
	{
		Button_Move = FLAGINDEX(1),

		Button_Hover = FLAGINDEX(0) + FLAGINDEX(1),

		Button_LClick = FLAGINDEX(2),
		Button_LHold = FLAGINDEX(3),
		Button_LDown = FLAGINDEX(2) + FLAGINDEX(3),
		Button_LUp = FLAGINDEX(4),

		Button_RClick = FLAGINDEX(5),
		Button_RHold = FLAGINDEX(6),
		Button_RDown = FLAGINDEX(5) + FLAGINDEX(6),
		Button_RUp = FLAGINDEX(7),

		Button_WDown = FLAGINDEX(8),
		Button_WUp = FLAGINDEX(9)
	};


	enum EMouseButton
	{
		MouseButton_None = FLAGINDEX(0),
		MouseButton_L = FLAGINDEX(1),
		MouseButton_R = FLAGINDEX(2),
		MouseButton_M = FLAGINDEX(3),
	};


	class AWindow;

	class AUserInterface;

	class AComponent;
	class AContainer;

	class AImage;

	class AText;

	class AButton;
	class ACheckbox;
	class ADragHandle;
	class ASlider;

	class ASingleChoiceBox;


	class AMenuBar;
	class AMenuBarEntry;


	class ADropDownMenu;
	class ADropDownMenuButton;
	class ADropDownMenuCheckbox;

	class APromt;


	struct FTexture;

	struct FButtonIcons;
	struct FCheckboxIcons;
	struct FSliderIcons;

	struct FCallback;

	struct FPromptResult;
}


class UI::AWindow
{
public:
	AWindow();
	virtual ~AWindow();

	friend class AUserInterface;


	SDL_Window* Window = nullptr;
	SDL_Renderer* Renderer = nullptr;


	FFlag<UShort>* PtrMouseEvents = nullptr;
	FPoint* PtrMousePosition = nullptr;


	FFlag<UShort> MouseEvents = 0;
	FPoint MousePosition = FPoint();

	UInt WindowFlags = 0;

	bool bTickAlways = false;

protected:
	AUserInterface* UserInterface = nullptr;


	vector<FTexture*> Textures;
	FRectangle Area;
	bool bWindowDimensionsChanged = false;
	bool bGhostWindowDimensionsChanged = true;
	bool bIsFocussed = false;

	AComponent* HoveredComponent = nullptr;
	bool bBlockUserInterfaceInput = false;

public:
	void SetUserInterface(AUserInterface* NewuserInterface);

	inline AUserInterface* GetUserInterface() const
	{
		return UserInterface;
	}

	void SetWindowIcon(string Path) const;

	void SetWindowTitle(string NewTitle) const;

	inline vector<FTexture*> GetAllTextures() const
	{
		return Textures;
	}

	inline FRectangle GetArea() const
	{
		return Area;
	}

	inline FPoint GetDimensions() const
	{
		return Area.GetDimensions();
	}

	bool GetWindowDimensionsChanged()
	{
		if (bWindowDimensionsChanged)
		{
			bWindowDimensionsChanged = false;
			return true;
		}
		return false;
	}

	inline bool GetIsFocussed() const
	{
		return bIsFocussed;
	}

	inline void SetMinimumSize(FSize Size) const
	{
		SDL_SetWindowMinimumSize(Window, Size.x, Size.y);
	}


	inline bool GetIsHovered() const
	{
		return HoveredComponent != nullptr;
	}

	inline AComponent* GetHoveredComponent() const
	{
		return HoveredComponent;
	}

	inline bool SetHoveredComponent(AComponent* NewHoveredComponent)
	{
		RETURN_IF(bBlockUserInterfaceInput) false;
		RETURN_IF(HoveredComponent == NewHoveredComponent) true;
		if (!HoveredComponent)
		{
			HoveredComponent = NewHoveredComponent;
			return true;
		}
		return false;
	}

	inline bool ResetHoveredComponent(AComponent* CurrentHoveredComponent)
	{
		if (HoveredComponent == CurrentHoveredComponent)
		{
			HoveredComponent = nullptr;
			return true;
		}
		return false;
	}

	inline void SetBlockUserInterfaceInput(bool NewBlocked)
	{
		bBlockUserInterfaceInput = NewBlocked;
		if (bBlockUserInterfaceInput)
		{
			HoveredComponent = nullptr;
		}
	}

	void ShowWindow() const
	{
		SDL_ShowWindow(Window);
	}

	void MaximizeWindow()
	{
		SDL_MaximizeWindow(Window);
		FPoint Dimensions;
		SDL_GetWindowPosition(Window, &Area.TL.x, &Area.TL.y);
		SDL_GetWindowSize(Window, &Dimensions.x, &Dimensions.y);
		Area.SetDimensions(Dimensions);
	}

public:

	FTexture* LoadTexture(string Path);

	void Init(string Name, FRectangle& InArea, FFlag<UShort>* InPtrMouseEvents, FPoint* InPtrMousePosition, UInt* WindowFlags = nullptr, UInt* RendererFlags = nullptr);


	virtual void Destroy();

	virtual void Render();
	virtual void Tick();
};


struct UI::FTexture
{
	SDL_Texture* Texture;
	string Path;

	FTexture()
	{
		Texture = nullptr;
		Path = "";
	}

	FTexture(SDL_Texture* InTexture, string InPath) : Texture(InTexture), Path(InPath)
	{
	}

	~FTexture()
	{
		SDL_DestroyTexture(Texture);
	}

	void operator=(FTexture& Other)
	{
		Texture = Other.Texture;
		Path = Other.Path;
	}

	void ClearTexture()
	{
		if (Texture)
		{
			SDL_DestroyTexture(Texture);
			Texture = nullptr;
		}
	}

	void LoadTexture(SDL_Renderer* Renderer)
	{
		Texture = IMG_LoadTexture(Renderer, Path.c_str());
	}

	void LoadTexture(SDL_Renderer* Renderer, string NewPath)
	{
		Path = NewPath;
		Texture = IMG_LoadTexture(Renderer, Path.c_str());
	}

	void ReloadTexture(SDL_Renderer* NewRenderer)
	{
		ClearTexture();
		LoadTexture(NewRenderer);
	}
};


struct UI::FButtonIcons
{
	FTexture* DefaultIcon = nullptr;
	FTexture* HoveredIcon = nullptr;
	FColor HoveredTint;
	FTexture* ActivatedIcon = nullptr;
	FColor ActivatedTint;


	bool bOverlay = false;
	bool bDefaultReactions = false;
	bool bTintBackground = false;

	FButtonIcons()
	{
	}

	FButtonIcons(FTexture* InDefaultIcon, FTexture* InHoveredIcon = nullptr, FTexture* InActivatedIcon = nullptr)
	{
		DefaultIcon = InDefaultIcon;
		HoveredIcon = InHoveredIcon;
		ActivatedIcon = InActivatedIcon;
	}

	FButtonIcons(bool InbOverlay, bool InbDefaultReactions, FTexture* InDefaultIcon, FTexture* InHoveredIcon = nullptr, FTexture* InActivatedIcon = nullptr)
	{
		bOverlay = InbOverlay;

		DefaultIcon = InDefaultIcon;
		HoveredIcon = InHoveredIcon;
		ActivatedIcon = InActivatedIcon;

		if (!HoveredIcon && !ActivatedIcon && InbDefaultReactions)
		{
			bDefaultReactions = true;
		}
	}

	FButtonIcons(AWindow* Window, string Path, bool InbOverlay = false, bool InbDefaultReactions = false, bool InbTintBackground = true)
	{
		bOverlay = InbOverlay;
		bDefaultReactions = InbDefaultReactions;
		bTintBackground = InbTintBackground;

		LoadFromFiles(Window, Path);
	}

	FButtonIcons(AWindow* Window, string Path, bool InbTintBackground, FColor InHoveredTint, FColor InActivatedTint)
	{
		bTintBackground = InbTintBackground;
		bDefaultReactions = true;

		HoveredTint = InHoveredTint;
		ActivatedTint = InActivatedTint;

		LoadFromFiles(Window, Path);
	}

	FButtonIcons(bool InbTintBackground, FColor InHoveredTint, FColor InActivatedTint)
	{
		bTintBackground = InbTintBackground;
		bDefaultReactions = true;

		HoveredTint = InHoveredTint;
		ActivatedTint = InActivatedTint;
	}

	void operator=(FButtonIcons& Other)
	{
		DefaultIcon = Other.DefaultIcon;
		HoveredIcon = Other.HoveredIcon;
		ActivatedIcon = Other.ActivatedIcon;

		bOverlay = Other.bOverlay;
		bDefaultReactions = Other.bDefaultReactions;
		bTintBackground = Other.bTintBackground;

		HoveredTint = Other.HoveredTint;
		ActivatedTint = Other.ActivatedTint;
	}

	void LoadFromFiles(AWindow* Window, string Path)
	{
		if (bDefaultReactions)
		{
			DefaultIcon = Window->LoadTexture(Path + ".png");
		}
		else
		{
			DefaultIcon = Window->LoadTexture(Path + "_D.png");
			HoveredIcon = Window->LoadTexture(Path + "_H.png");
			ActivatedIcon = Window->LoadTexture(Path + "_A.png");
		}
	}

	void LoadFromFiles(AWindow* Window, string Path, string Extension)
	{
		if (bDefaultReactions)
		{
			DefaultIcon = Window->LoadTexture(Path + Extension + ".png");
		}
		else
		{
			DefaultIcon = Window->LoadTexture(Path + "_D" + Extension + ".png");
			HoveredIcon = Window->LoadTexture(Path + "_H" + Extension + ".png");
			ActivatedIcon = Window->LoadTexture(Path + "_A" + Extension + ".png");
		}
	}

	void UpdateTints(FColor NewDefaultTint, FColor NewHoveredTint, FColor NewActivatedTint);
	void UpdateAlpha(UChar NewDefaultAlpha, UChar NewHoveredAlpha, UChar NewActivatedAlpha);
};


struct UI::FCheckboxIcons
{
	FButtonIcons Checked;
	FButtonIcons UnChecked;

	int BorderThickness = 0;
	FColor BorderColor;

	bool bDefaultCheckbox = false;

	FCheckboxIcons()
	{
	}

	FCheckboxIcons(FColor InHoveredColor, FColor InActivatedColor, FColor InBorderColor, int InBorderThickness = 1)
	{
		Checked.HoveredTint = InHoveredColor;
		Checked.ActivatedTint = InActivatedColor;
		BorderColor = InBorderColor;
		BorderThickness = InBorderThickness;
		bDefaultCheckbox = true;
	}

	FCheckboxIcons(FButtonIcons InChecked, FButtonIcons InUnChecked)
	{
		bDefaultCheckbox = false;

		Checked = InChecked;
		UnChecked = InUnChecked;
	}

	FCheckboxIcons(AWindow* Window, string Path, bool InbOverlay = false, bool InbDefaultReactions = false, bool InbTintBackground = true)
	{
		bDefaultCheckbox = false;

		LoadFromFiles(Window, Path, InbOverlay, InbDefaultReactions, InbTintBackground);
	}

	FCheckboxIcons(AWindow* Window, string Path, bool InbTintBackground, FColor InHoveredTint, FColor InActivatedTint, FColor InBorderColor, int InBorderThickness = 1)
	{
		bDefaultCheckbox = false;

		Checked.HoveredTint = InHoveredTint;
		Checked.ActivatedTint = InActivatedTint;

		UnChecked.HoveredTint = InHoveredTint;
		UnChecked.ActivatedTint = InActivatedTint;


		BorderThickness = InBorderThickness;
		BorderColor = InBorderColor;

		LoadFromFiles(Window, Path, false, true, InbTintBackground);
	}

	void operator=(FCheckboxIcons& Other)
	{
		Checked = Other.Checked;
		UnChecked = Other.UnChecked;
		BorderColor = Other.BorderColor;
		BorderThickness = Other.BorderThickness;
		bDefaultCheckbox = Other.bDefaultCheckbox;
	}

	void LoadFromFiles(AWindow* Window, string Path, bool InbOverlay = false, bool InbDefaultReactions = false, bool InbTintBackground = true)
	{
		Checked.bOverlay = InbOverlay;
		Checked.bDefaultReactions = InbDefaultReactions;
		Checked.bTintBackground = InbTintBackground;

		UnChecked.bOverlay = InbOverlay;
		UnChecked.bDefaultReactions = InbDefaultReactions;
		UnChecked.bTintBackground = InbTintBackground;

		if (InbDefaultReactions)
		{
			Checked.LoadFromFiles(Window, Path);
			UnChecked.DefaultIcon = Checked.DefaultIcon;
		}
		else
		{
			UnChecked.LoadFromFiles(Window, Path, "_F");
			Checked.LoadFromFiles(Window, Path, "_T");
		}
	}

	void UpdateTints(FColor NewDefaultTint, FColor NewHoveredTint, FColor NewActivatedTint, FColor NewBorderColor);
	void UpdateAlpha(UChar NewDefaultAlpha, UChar NewHoveredAlpha, UChar NewActivatedAlpha, UChar NewBorderAlpha);
};


struct UI::FSliderIcons
{
	FButtonIcons Button;
	FTexture* Background;

	FSliderIcons(): Background(nullptr)
	{
	}

	FSliderIcons(FButtonIcons InButton, FTexture* InBackground)
	{
		Button = InButton;
		Background = InBackground;
	}

	FSliderIcons(AWindow* Window, string Path)
	{
		LoadFromFiles(Window, Path);
	}

	void operator=(FSliderIcons& Other)
	{
		Button = Other.Button;
		Background = Other.Background;
	}

	void LoadFromFiles(AWindow* Window, string Path)
	{
		Button.LoadFromFiles(Window, Path + "_B");
		Background = Window->LoadTexture(Path + "_D.png");
	}
};


struct UI::FCallback
{
	boost::function<void()> Callback;
	bool bTriggerOnActivated;


	FCallback(boost::function<void()> InCallback, bool InbTriggerOnActivated = true)
	{
		Callback = InCallback;
		bTriggerOnActivated = InbTriggerOnActivated;
	}

	FCallback(bool InbTriggerOnActivated = true)
	{
		bTriggerOnActivated = InbTriggerOnActivated;
	}
};


struct UI::FPromptResult
{
	UShort Index;
	wstring ButtonText;
	bool bWasClosed;
	bool bWaiting;

	FPromptResult()
	{
		Index = 0;
		ButtonText = L"";
		bWasClosed = false;
		bWaiting = true;
	}

	FPromptResult(UShort InIndex, wstring InButtonText, bool InbWasClosed = true)
	{
		Index = InIndex;
		ButtonText = InButtonText;
		bWasClosed = InbWasClosed;
		bWaiting = false;
	}

	FPromptResult(bool InbWasClosed)
	{
		Index = 0;
		ButtonText = L"";
		bWasClosed = bWaiting = InbWasClosed;
	}

	inline void ReceivedAnswer()
	{
		bWaiting = false;
	}
};


class UI::AComponent
{
public:
	AComponent(AUserInterface* InParentUserInterface, FRectangle InArea);
	virtual ~AComponent();

	friend class AContainer;

	virtual void Render(SDL_Renderer* Renderer);
	virtual void Tick();


	FRectangle Padding;

	inline FRectangle GetPaddedArea() const
	{
		return FRectangle(Area.TL - Padding.TL, Area.BR + Padding.BR);
	}

	AUserInterface* const ParentUserInterface;
	AContainer* ParentContainer;

	FFlag<UShort> Visibility = UI_Visible;
	FFlag<UShort> Flags;


	inline FPoint GetRelativePosition(FPoint Offset) const
	{
		return Area.TL + Offset;
	}

	inline FPoint GetRelativePosition(int x = 0, int y = 0) const
	{
		return Area.TL + FPoint(x, y);
	}

	inline FRectangle GetArea() const
	{
		return Area;
	}

	inline FPoint GetDimensions() const
	{
		return Area.GetDimensions();
	}

	virtual void SetDimensions(FPoint NewDimensions);
	virtual void SetAnchor(FPoint NewAnchor);
	virtual void ChangeAnchor(FPoint DeltaAnchor);
	virtual void SetHorizontalAnchor(int x);
	virtual void SetVerticalAnchor(int y);

protected:
	FRectangle Area;
};


class UI::AContainer : public AComponent
{
public:
	AContainer(AUserInterface* InParentUserInterface, FRectangle InArea);
	virtual ~AContainer();

	void Tick() override;
	void Render(SDL_Renderer* Renderer) override;


	void SetAnchor(FPoint NewAnchor) override;
	void ChangeAnchor(FPoint DeltaAnchor) override;

	virtual AComponent* AddComponent(AComponent* Component, int Index = -1);
	virtual void RemoveComponent(AComponent* Component);

	inline vector<AComponent*> GetComponents() const
	{
		return Components;
	}

	inline void UpdateComponents()
	{
		bUpdatePositions = true;
	}

	void UpdatePositions();

	inline void SetVisibility(EUserInterfaceFlags NewVisibility)
	{
		Visibility = NewVisibility;
		for each (AComponent* CurrentComponent in Components)
		{
			CurrentComponent->Visibility = Visibility;
		}
	}

protected:
	vector<AComponent*> Components;

	bool bUpdatePositions = true;
};


class UI::AImage : public AComponent
{
public:
	AImage(AUserInterface* InParentUserInterface, FRectangle InArea, FTexture* InTexture, float InAngle = 0.f);
	virtual ~AImage();

	//virtual void Tick();
	void Render(SDL_Renderer* Renderer) override;

	FTexture* Texture;

	float Angle;
};


class UI::AText : public AComponent
{
public:
	AText(AUserInterface* InParentUserInterface, FRectangle InArea, TTF_Font* InFont = nullptr, wstring InText = L"", FColor InColor = FColor(255), float InOffset = 1.f);
	AText(AUserInterface* InParentUserInterface, FRectangle InArea, TTF_Font* InFont, wstring* InTextPtr = nullptr, FColor InColor = FColor(255), float InOffset = 1.f);
	virtual ~AText();

	void Tick() override;
	void Render(SDL_Renderer* Renderer) override;


	TTF_Font* Font;
	FColor Color;

	float Offset;

protected:
	wstring Text;
	wstring* TextPtr;
	bool bTextWasChanged;
	bool bMultiLine = false;


public:
	void SetTextPointer(wstring* NewTextPointer);

	inline void ResetTextPointer()
	{
		TextPtr = nullptr;
	}

	inline wstring* GetTextPointer() const
	{
		return TextPtr;
	}

	inline void SetMultiLine(bool NewbMultiLine)
	{
		bMultiLine = true;
	}

	inline bool GetMultiLine() const
	{
		return bMultiLine;
	}

	void SetText(wstring NewText);

	inline wstring GetText() const
	{
		return Text;
	}

	inline bool GetTextWasChanged() const
	{
		return bTextWasChanged;
	}
};


class UI::AButton : public AComponent
{
public:
	friend class ASlider;


	AButton(AUserInterface* InParentUserInterface, FRectangle InArea, FButtonIcons InIcons);
	virtual ~AButton();

	FButtonIcons Icons;


	bool bIsHovered = false;
	bool bIsActivated = false;

	bool bSizeToText = false;


	AButton* AddEventCallback(FFlag<UShort> ButtonEvent, FCallback Callback);
	AButton* AddEventCallback(FFlag<UShort> ButtonEvent, boost::function<void()> Callback, bool bTriggerOnActivated = true);
	AButton* AddEvent(FFlag<UShort> ButtonEvent, bool bTriggerOnActivated = true);

	void Tick() override;

	void Render(SDL_Renderer* Renderer) override;


	virtual void AddTextComponent(AText* NewTextComponent);
	virtual void AddTextComponent(FPoint Offset, TTF_Font* InFont, wstring InText, FColor InColor, float TextCenterOffset = 1.f);
	virtual void AddTextComponent(FPoint Offset, TTF_Font* InFont, wstring* InTextPtr, FColor InColor, float TextCenterOffset = 1.f);

	inline AText* GetTextComponent() const
	{
		return TextComponent;
	}


	//virtual void SetDimensions(FPoint NewDimensions)
	//{
	//	FPoint OldDimensions = Area.GetDimensions();
	//	AComponent::SetDimensions(NewDimensions);
	//	//IconArea.SetDimensions(IconArea.GetDimensions() - (OldDimensions - NewDimensions));
	//}
	void SetAnchor(FPoint NewAnchor) override
	{
		FPoint OldAnchor = Area.TL;
		AComponent::SetAnchor(NewAnchor);
		DrawArea.TL += NewAnchor - OldAnchor;
		DrawArea.BR += NewAnchor - OldAnchor;

		if (TextComponent)
		{
			TextComponent->ChangeAnchor(NewAnchor - OldAnchor);
		}
	}

	void ChangeAnchor(FPoint DeltaAnchor) override
	{
		SetAnchor(Area.TL + DeltaAnchor);
	}

	void SetHorizontalAnchor(int x) override
	{
		ChangeAnchor(FPoint(x - Area.TL.x, 0));
	}

	void SetVerticalAnchor(int y) override
	{
		ChangeAnchor(FPoint(0, y - Area.TL.y));
	}


	virtual inline void SetIconArea(FRectangle NewIconArea)
	{
		IconArea = NewIconArea;
		DrawArea = FRectangle(Area.TL + IconArea.TL, Area.TL + IconArea.BR);
	}

	virtual inline void SetIconDimensions(FPoint NewIconDimensions)
	{
		IconArea.SetDimensions(NewIconDimensions);
		DrawArea.BR = Area.TL + IconArea.BR;
	}

protected:
	map<UShort, FCallback> Callbacks;

	virtual void OnActivated(const bool Callback = false);

	FRectangle IconArea;

	FRectangle DrawArea;

	FTexture* Icon;

	AText* TextComponent;


	inline void UpdateIcon()
	{
		Icon = bIsHovered ? (bIsActivated ? Icons.ActivatedIcon : Icons.HoveredIcon) : Icons.DefaultIcon;
	}

	virtual void OnTextAdded()
	{
	}
};


class UI::ACheckbox : public AButton
{
public:
	ACheckbox(AUserInterface* InParentUserInterface, FRectangle InArea, FCheckboxIcons InCheckboxIcons, const bool InbIsChecked = false);
	virtual ~ACheckbox();

	void SetIsChecked(const bool NewIsChecked);

	bool GetIsChecked() const
	{
		return bIsChecked;
	}

	FCheckboxIcons CheckboxIcons;

	bool bAllowDeactivate = true;

	void Tick() override;

	void Render(SDL_Renderer* Renderer) override;


protected:
	bool bIsChecked;

	void OnActivated(const bool Callback = false) override;

public:
	bool* GetCheckedPointer()
	{
		return &bIsChecked;
	}
};


class UI::ADragHandle : public AComponent
{
public:
	ADragHandle(AUserInterface* InParentUserInterface, FRectangle InArea, FColor InColor, bool InbIsHorizontal = false, int InDragMin = -INT_MAX, int InDragMax = INT_MAX);
	virtual ~ADragHandle();

	void Tick() override;
	void Render(SDL_Renderer* Renderer) override;

	FColor Color;

	FFlag<UShort> MouseButtons;


protected:
	FFlag<UShort> StartEvents;
	FFlag<UShort> EndEvents;

	AComponent* ControlledComponent;
	int ComponentOffset = 0;


	bool bIsHorizontal;
	int DragMax;
	int DragMin;

	bool bWasChanged = false;

	int Position = 0;


	bool bIsHovered = false;
	bool bIsActivated = false;

public:

	void SetPosition(int NewPosition);

	inline void SetControlledComponent(AComponent* Component)
	{
		ControlledComponent = Component;
	}

	inline void SetComponentOffset(int NewOffset)
	{
		ComponentOffset = NewOffset;
	}

	ADragHandle* AddMouseButton(EMouseButton NewButton)
	{
		MouseButtons.Add(NewButton);
		if (MouseButtons.Has(MouseButton_L))
		{
			StartEvents.Add(Mouse_LHold);
			EndEvents.Add(Mouse_LClick | Mouse_LUp);
		}
		if (MouseButtons.Has(MouseButton_R))
		{
			StartEvents.Add(Mouse_RHold);
			EndEvents.Add(Mouse_RClick | Mouse_RUp);
		}
		return this;
	}

	inline void SetDragMin(int New)
	{
		DragMin = New;
		SetPosition(Position);
	}

	inline void SetDragMax(int New)
	{
		DragMax = New;
		SetPosition(Position);
	}

	inline void SetDragRange(int NewMin, int NewMax)
	{
		DragMax = NewMax;
		DragMin = NewMin;
		SetPosition(Position);
	}

	inline int GetPosition() const
	{
		return Position;
	}

	inline bool GetWasChanged()
	{
		return bWasChanged ? !((bWasChanged = false)) : false;
	}
};


class UI::ASlider : public AComponent
{
public:
	ASlider(AUserInterface* InParentUserInterface, FRectangle InArea, FPoint InButtonSize, FSliderIcons InIcons, float InPrecentage = 0.5f);
	virtual ~ASlider();

	void Tick() override;
	void Render(SDL_Renderer* Renderer) override;


	AButton* const Button;
	FSliderIcons Icons;


	inline void SetDimensions(FPoint NewDimensions) override
	{
		AComponent::SetDimensions(NewDimensions);
	}

	void SetAnchor(FPoint NewAnchor) override
	{
		FPoint OldAnchor = Area.TL;
		AComponent::SetAnchor(NewAnchor);
		IconArea.SetAnchor(IconArea.TL + (NewAnchor - OldAnchor));
		Button->SetAnchor(NewAnchor);
	}

	inline void ChangeAnchor(FPoint DeltaAnchor) override
	{
		SetAnchor(Area.TL + DeltaAnchor);
	}

	inline void SetHorizontalAnchor(int x) override
	{
		ChangeAnchor(FPoint(x - Area.TL.x, 0));
	}

	inline void SetVerticalAnchor(int y) override
	{
		ChangeAnchor(FPoint(0, y - Area.TL.y));
	}


protected:
	bool bDragged;
	float Percentage;
	bool bWasChanged;

	FRectangle IconArea;


public:

	void SetPercentage(float NewPercentage)
	{
		Percentage = Utils::Clamp(NewPercentage, 0.f, 1.f);
		IconArea.SetHorizontalAnchor(int(round(Utils::Lerp(Area.TL.x, Area.BR.x - IconArea.GetDimensions().x, Percentage))));
		bWasChanged = true;
	}

	inline float GetPercentage() const
	{
		return Percentage;
	}

	inline bool GetIsDragged() const
	{
		return bDragged;
	}

	bool GetWasChanged()
	{
		if (bWasChanged)
		{
			bWasChanged = false;
			return true;
		}
		return bWasChanged = false;
	}
};


class UI::ASingleChoiceBox : public AContainer
{
public:
	ASingleChoiceBox(AUserInterface* InParentUserInterface, FRectangle InArea);
	virtual ~ASingleChoiceBox();

	void Tick() override;

	inline bool GetIsSet() const
	{
		return CurrentCheckedBox != nullptr;
	}

	inline bool GetAllowDeactivate() const
	{
		return bAllowDeactivate;
	}

	void SetAllowDeactivate(bool NewAllowDeactivate);

	virtual ACheckbox* AddComponent(ACheckbox* Checkbox, int Index = -1);


private:

	bool bAllowDeactivate = false;

	ACheckbox* CurrentCheckedBox = nullptr;
};


class UI::ADropDownMenu : public AContainer
{
public:


	ADropDownMenu(AUserInterface* InParentUserInterface, FPoint InEntryDimensions, FColor InBackgroundColor = FColor(0), FColor InBorderColor = FColor(0));
	virtual ~ADropDownMenu();

	void Tick() override;
	void Render(SDL_Renderer* Renderer) override;


	virtual ADropDownMenuButton* AddButton(ADropDownMenuButton* Component, int Index = -1);
	virtual ADropDownMenuCheckbox* AddCheckbox(ADropDownMenuCheckbox* Component, int Index = -1);


	bool bAutoClose = true;

	FColor BackgroundColor;
	FColor BorderColor;

private:
	bool bIsOpen = false;
	bool bIsHovered = false;

	FPoint EntryDimensions;

	virtual AComponent* AComponent(AComponent* Component, int Index = -1)
	{
		PRINT "Adding elements other than dropdown-specific Elements is not allowed" END;
		assert(false);
		return nullptr;
	}


public:

	int AutoCloseDistance = 20;

	inline FPoint GetEntryDimensions() const
	{
		return EntryDimensions;
	}

	bool SetOpen(bool NewbIsOpen);

	inline void Open()
	{
		SetOpen(true);
	}

	inline void Close()
	{
		SetOpen(false);
	}

	inline bool ToggleOpen()
	{
		return SetOpen(!bIsOpen);
	}

	inline bool GetIsOpen() const
	{
		return bIsOpen;
	}

	inline bool GetIsHovered() const
	{
		return bIsHovered;
	}
};


class UI::ADropDownMenuButton : public AButton
{
public:
	ADropDownMenuButton(AUserInterface* InParentUserInterface, FRectangle InArea, FButtonIcons InIcons);
	virtual ~ADropDownMenuButton();

	void Render(SDL_Renderer* Renderer) override;

	virtual void AddTextComponent(TTF_Font* InFont, wstring InText, FColor InColor);
	virtual void AddTextComponent(TTF_Font* InFont, wstring* InTextPtr, FColor InColor);

	bool bCloseOnClicked = true;

	void OnActivated(const bool Callback = false) override;


protected:
	void OnTextAdded() override;
};


class UI::ADropDownMenuCheckbox : public ACheckbox
{
public:
	ADropDownMenuCheckbox(AUserInterface* InParentUserInterface, FRectangle InArea, FCheckboxIcons InIcons);
	virtual ~ADropDownMenuCheckbox();

	void Render(SDL_Renderer* Renderer) override;

	virtual void AddTextComponent(TTF_Font* InFont, wstring InText, FColor InColor);
	virtual void AddTextComponent(TTF_Font* InFont, wstring* InTextPtr, FColor InColor);

	bool bCloseOnClicked = false;

	void OnActivated(const bool Callback = false) override;

	void UpdateCheckBoxArea()
	{
		CheckBoxArea = FRectangle(Area.TL + FPoint(5), FPoint(Area.TL.x + Area.GetHeight() - 5, Area.BR.y - 5));
	}

protected:
	FRectangle CheckBoxArea;

	void OnTextAdded() override;
};


class UI::AMenuBar : public AContainer
{
public:
	AMenuBar(AUserInterface* InParentUserInterface, FRectangle InArea, TTF_Font* InFont, FSize InEntrySize, FColor InColor, FColor InTextColor, FColor InHoveredColor, FColor InActivatedColor, FColor InOpendedColor);
	virtual ~AMenuBar();

	friend class AMenuBarEntry;

	void Tick() override;

	void Render(SDL_Renderer* Renderer) override;

	FColor Color;
	FColor TextColor;
	FColor HoveredColor;
	FColor ActivatedColor;
	FColor OpendedColor;

	FSize EntrySize;

	TTF_Font* Font;

	AMenuBarEntry* AddEntry(wstring Name);
	AMenuBarEntry* AddEntry(wstring* NamePtr);

	AMenuBarEntry* GetEntry(wstring Name) const;
	AMenuBarEntry* GetEntry(wstring* NamePtr) const;
private:
	AMenuBarEntry* OpenedEntry = nullptr;
};


class UI::AMenuBarEntry : public AComponent
{
public:
	AMenuBarEntry(AUserInterface* InParentUserInterface, FSize InEntrySize, AMenuBar* InMenuBar, TTF_Font* InFont, wstring InName);
	AMenuBarEntry(AUserInterface* InParentUserInterface, FSize InEntrySize, AMenuBar* InMenuBar, TTF_Font* InFont, wstring* InNamePtr);
	virtual ~AMenuBarEntry();

	friend class AMenuBar;

	void Tick() override;

	void Render(SDL_Renderer* Renderer) override;

	void SetName(wstring NewName);

private:
	ADropDownMenu* DropDownMenu;

	wstring Name;
	wstring* NamePtr;
	TTF_Font* Font;

	AMenuBar* MenuBar;


	FSize Size;
	int Anchor;

	bool bIsHovered = false;
	bool bIsActivated = false;
	bool bIsOpended = false;

	bool bCloseDelay = 0;
public:

	bool SetOpen(bool NewbIsOpen)
	{
		bIsOpended = NewbIsOpen;
		DropDownMenu->SetOpen(bIsOpended);
		//	DropDownMenu->SetVisibility(bIsOpended ? UI_Visible : UI_Hidden);
		return bIsOpended;
	}

	void Open()
	{
		SetOpen(true);
	}

	void Close()
	{
		SetOpen(false);
	}

	bool ToggleOpen()
	{
		return SetOpen(!bIsOpended);
	}

	bool GetIsOpen() const
	{
		return bIsOpended;
	}

	virtual ADropDownMenuButton* AddButtonWithText(FButtonIcons Icons, TTF_Font* Font, wstring Text, FColor Color = FColor(), int Index = -1)
	{
		ADropDownMenuButton* NewButton = AddButton(new ADropDownMenuButton(ParentUserInterface, FRectangle(), Icons), Index);
		NewButton->AddTextComponent(Font, Text, Color);
		return NewButton;
	}

	virtual ADropDownMenuButton* AddButtonWithText(FButtonIcons Icons, TTF_Font* Font, wstring* TextPtr, FColor Color = FColor(), int Index = -1)
	{
		ADropDownMenuButton* NewButton = AddButton(new ADropDownMenuButton(ParentUserInterface, FRectangle(), Icons), Index);
		NewButton->AddTextComponent(Font, TextPtr, Color);
		return NewButton;
	}

	virtual ADropDownMenuButton* AddButton(ADropDownMenuButton* Component, int Index = -1)
	{
		return DropDownMenu->AddButton(Component, Index);
	}


	virtual ADropDownMenuCheckbox* AddCheckboxWithText(FCheckboxIcons Icons, TTF_Font* Font, wstring Text, FColor Color = FColor(), int Index = -1)
	{
		ADropDownMenuCheckbox* NewButton = AddCheckbox(new ADropDownMenuCheckbox(ParentUserInterface, FRectangle(), Icons), Index);
		NewButton->AddTextComponent(Font, Text, Color);
		return NewButton;
	}

	virtual ADropDownMenuCheckbox* AddCheckboxWithText(FCheckboxIcons Icons, TTF_Font* Font, wstring* TextPtr, FColor Color = FColor(), int Index = -1)
	{
		ADropDownMenuCheckbox* NewButton = AddCheckbox(new ADropDownMenuCheckbox(ParentUserInterface, FRectangle(), Icons), Index);
		NewButton->AddTextComponent(Font, TextPtr, Color);
		return NewButton;
	}

	virtual ADropDownMenuCheckbox* AddCheckbox(ADropDownMenuCheckbox* Component, int Index = -1)
	{
		return DropDownMenu->AddCheckbox(Component, Index);
	}

	ADropDownMenu* GetDropDownMenu() const
	{
		return DropDownMenu;
	}
};


class UI::APromt : public AContainer
{
public:
	APromt(AUserInterface* InParentUserInterface, FRectangle InArea, TTF_Font* InFont, wstring Message, wstring Buttons, FColor InColor, FColor InTextColor, FColor InHoveredColor, FColor InActivatedColor, FColor InOpendedColor);
	virtual ~APromt();

	friend class AMenuBarEntry;

	void Tick() override;

	void Render(SDL_Renderer* Renderer) override;

	void GetInput(FCallback Callback);


	FColor Color;
	FColor TextColor;
	FColor HoveredColor;
	FColor ActivatedColor;

	TTF_Font* Font;

private:
	FPromptResult Result;
	bool bIsActive = false;

	void WaitForInput(FCallback Callback);
	void ReceiveInput(AButton* Button);
};


class UI::AUserInterface : public AContainer
{
public:
	AUserInterface(AWindow* InParentWindow);
	virtual ~AUserInterface();

	friend class AWindow;

	AWindow* ParentWindow;

	void Tick() override;
	void Render(SDL_Renderer* Renderer) override;

	virtual void Init();


	virtual void UpdateAreas()
	{
	}


	inline bool GetIsHovered() const
	{
		return ParentWindow->GetIsHovered();
	}

	inline AComponent* GetHoveredComponent() const
	{
		return ParentWindow->HoveredComponent;
	}

	inline bool SetHoveredComponent(AComponent* NewHoveredComponent) const
	{
		return ParentWindow->SetHoveredComponent(NewHoveredComponent);
	}

	inline bool ResetHoveredComponent(AComponent* CurrentHoveredComponent) const
	{
		return ParentWindow->ResetHoveredComponent(CurrentHoveredComponent);
	}


private:

	void ParentWindowAreaUpdate()
	{
		Area = ParentWindow->GetArea();
		UpdateAreas();
	}
};


#endif
