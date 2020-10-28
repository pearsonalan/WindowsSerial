// Copyright(C) 2020 Alan Pearson
//
// This program is free software : you can redistribute it and /or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.If not, see < https://www.gnu.org/licenses/>.

#include "PreferencesDialog.h"
#include "Registry.h"
#include "Monitor.h"

LRESULT PreferencesDialog::onInitDialog(HWND hwndParam, HWND hwndFocus, LPARAM lParam) {
	winfx::DebugOut(L"PreferencesDialog::onInitDialog");
	RegistryKey settings_key = RegistryKey::CurrentUser.openOrCreate(kRegistryKeyName);
	if (settings_key != RegistryKey::InvalidKey) {
		std::wstring com_port = settings_key.getStringValueOrEmptyString(L"Port");
		winfx::DebugOut(L"COM Port is %s", com_port.c_str());
		setItemText(IDC_COM_PORT, com_port);
	}
	return TRUE;
}

void PreferencesDialog::onOk() {
	std::wstring com_port = getItemText(IDC_COM_PORT);
	winfx::DebugOut(L"On OK. ComPort = %s", com_port.c_str());
	RegistryKey settings_key = RegistryKey::CurrentUser.openOrCreate(kRegistryKeyName);
	settings_key.setStringValue(L"Port", com_port);
}