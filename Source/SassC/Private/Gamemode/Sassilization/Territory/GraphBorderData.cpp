// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#include "Gamemode/Sassilization/Territory/GraphBorderData.h"
#include "Gamemode/Sassilization/Territory/GraphBorder.h"
#include "Gamemode/Sassilization/Territory/GraphNode.h"

UGraphBorder* UGraphBorderData::GetBorder() const
{
	return Border;
}

void UGraphBorderData::SetBorder(UGraphBorder* NewBorder)
{
	this->Border = NewBorder;
}

AGraphNode* UGraphBorderData::GetNext() const
{
	return Next;
}

void UGraphBorderData::SetNext(AGraphNode* NewNext)
{
	this->Next = NewNext;
}

AGraphNode* UGraphBorderData::GetPrevious() const
{
	return Previous;
}

void UGraphBorderData::SetPrevious(AGraphNode* NewPrevious)
{
	this->Previous = NewPrevious;
}

FString UGraphBorderData::ToString() const
{
	return "[BorderData Name: " + GetName() + 
		", Border: " + (Border ? Border->ToString() : "NULL") + 
		", Next: " + (Next ? Next->GetName() : "NULL") +
		", Previous: " + (Previous ? Previous->GetName() : "NULL") + 
		"]";
}
