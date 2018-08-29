#include "stdafx.h"
#include "AimpDlnaGroupingTreeDataProviderSelection.h"
#include "AimpDlnaMusicLibrary.h"

HRESULT WINAPI AimpDlnaGroupingTreeDataProviderSelection::GetDisplayValue(IAIMPString** S) {
	if (GetCurrentNode() == nullptr)
		return E_FAIL;

	*S = AimpString(GetCurrentNode()->DisplayTitle, true);
	return S_OK;
}

DWORD WINAPI AimpDlnaGroupingTreeDataProviderSelection::GetFlags() {
	if (GetCurrentNode() == nullptr)
		return E_FAIL;

	int flags = 0;
	if (GetCurrentNode()->Standalone)
		flags |= AIMPML_GROUPINGTREENODE_FLAG_STANDALONE;
	if (GetCurrentNode()->HasChildren)
		flags |= AIMPML_GROUPINGTREENODE_FLAG_HASCHILDREN;

	return flags;
}

HRESULT WINAPI AimpDlnaGroupingTreeDataProviderSelection::GetImageIndex(int* Index) {
	if (GetCurrentNode() == nullptr)
		return E_FAIL;

	*Index = GetCurrentNode()->ImageIndex;
	return S_OK;
}

HRESULT WINAPI AimpDlnaGroupingTreeDataProviderSelection::GetValue(IAIMPString** FieldName, VARIANT* Value) {
	if (GetCurrentNode() == nullptr)
		return E_FAIL;

	*FieldName = AimpString(EVDS_NodeId, true);
	VariantCopyInd(Value, &_variant_t(GetCurrentNode()->Value.c_str()));
	return S_OK;
}

BOOL WINAPI AimpDlnaGroupingTreeDataProviderSelection::NextRow() {
	if (nodes.size() == 0)
		return false;

	index++;
	bool result = index < nodes.size();
	if (!result)
		index = 0;

	return result;
}

HRESULT WINAPI AimpDlnaGroupingTreeDataProviderSelection::QueryInterface(REFIID riid, LPVOID* ppvObject) {
	if (riid == IID_IAIMPMLGroupingTreeDataProviderSelection) {
		*ppvObject = this;
		AddRef();
		return S_OK;
	}

	return Base::QueryInterface(riid, ppvObject);
}