// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerListItem.h"
#include "MainMenu.h"
#include "Components/Button.h"

void UServerListItem::Setup(class UMainMenu* ParentObject, uint32 IndexValue)
{
    this->Index = IndexValue;
    this->Parent = ParentObject;
    ServerButton->OnClicked.AddDynamic(this, &UServerListItem::OnClicked);
}

void UServerListItem::OnClicked()
{
    if (Parent != nullptr)
    {
        Parent->SelectIndex(Index);
    }
}
