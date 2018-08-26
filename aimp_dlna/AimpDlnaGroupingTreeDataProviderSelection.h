#pragma once

struct AimpDlnaGroupingTreeDataProviderNode {
	int ImageIndex;
	wstring Value;
	wstring DisplayTitle;
	bool Standalone;
	bool HasChildren;
};

class AimpDlnaGroupingTreeDataProviderSelection : public IUnknownInterfaceImpl<IAIMPMLGroupingTreeDataProviderSelection> {
private:
	int index;
	vector<AimpDlnaGroupingTreeDataProviderNode> nodes;

	inline AimpDlnaGroupingTreeDataProviderNode* GetCurrentNode() {
		return index >= nodes.size() ? nullptr : &nodes[index];
	}

public:
	AimpDlnaGroupingTreeDataProviderSelection(vector<AimpDlnaGroupingTreeDataProviderNode> nodes) {
		this->nodes = nodes;
		this->index = 0;
	}

	HRESULT WINAPI GetDisplayValue(IAIMPString** S);
	DWORD WINAPI GetFlags();
	HRESULT WINAPI GetImageIndex(int* Index);
	HRESULT WINAPI GetValue(IAIMPString** FieldName, VARIANT* Value);
	BOOL WINAPI NextRow();
};