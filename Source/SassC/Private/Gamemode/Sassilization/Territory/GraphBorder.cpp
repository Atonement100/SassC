// License information can be found at https://github.com/Atonement100/SassC/blob/master/LICENSE

#include "Gamemode/Sassilization/Territory/GraphBorder.h"
#include "Gamemode/Sassilization/Territory/GraphNode.h"

AGraphNode* UGraphBorder::GetHead() const
{
	return Head;
}

void UGraphBorder::SetHead(AGraphNode* NewHead)
{
	this->Head = NewHead;
}

AGraphNode* UGraphBorder::GetTail() const
{
	return Tail;
}

void UGraphBorder::SetTail(AGraphNode* NewTail)
{
	this->Tail = NewTail;
}

FString UGraphBorder::ToString() const
{	
	return  "[Head: " + (Head ? Head->GetName() : "NULL") +
			", Tail: " + (Tail ? Tail->GetName() : "NULL") +
			"]";
}
