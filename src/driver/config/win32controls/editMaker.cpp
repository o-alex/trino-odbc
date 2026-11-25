#include "editMaker.hpp"

#include "../../../util/stringFromChar.hpp"

HWND editMaker(HWND form, int vertOffset, int editId, bool visible) {
  DWORD visibleStyle = visible ? WS_VISIBLE : 0;
  // Include WS_TABSTOP so that the user can use tab to cycle
  // through the edit elements in the form.
  return CreateWindow("EDIT",
                      "",
                      visibleStyle | WS_CHILD | WS_BORDER | WS_TABSTOP |
                          WS_VSCROLL | ES_MULTILINE,
                      165,
                      vertOffset,
                      425,
                      20,
                      form,
                      reinterpret_cast<HMENU>(static_cast<UINT_PTR>(editId)),
                      GetModuleHandle(NULL),
                      NULL);
}

HWND passwordEditMaker(HWND form, int vertOffset, int editId, bool visible) {
  DWORD visibleStyle = visible ? WS_VISIBLE : 0;
  // ES_PASSWORD has no effect on ES_MULTILINE edits, so this control is
  // single-line, unlike the ones from editMaker.
  return CreateWindow("EDIT",
                      "",
                      visibleStyle | WS_CHILD | WS_BORDER | WS_TABSTOP |
                          ES_PASSWORD | ES_AUTOHSCROLL,
                      165,
                      vertOffset,
                      425,
                      20,
                      form,
                      reinterpret_cast<HMENU>(static_cast<UINT_PTR>(editId)),
                      GetModuleHandle(NULL),
                      NULL);
}

std::string getEditText(HWND edit) {
  char currentText[1024];
  GetWindowText(edit, currentText, sizeof(currentText));
  return stringFromChar(currentText, CHAR_IS_NTS);
}

void setEditText(HWND edit, std::string text) {
  char currentText[1024];
  GetWindowText(edit, currentText, sizeof(currentText));
  if (text != currentText) {
    SetWindowText(edit, text.c_str());
  }
}

void setEditReadOnly(HWND edit) {
  SendMessage(edit, EM_SETREADONLY, TRUE, 0);
}

void setEditWriteable(HWND edit) {
  SendMessage(edit, EM_SETREADONLY, FALSE, 0);
}
