#include "lrtimer.h"
#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include <time.h>



#define OFFSET 5


long msMultiplayer = 0;
float timeDelay = 4;
BOOL toggleOn = false;
LRTimer timer;
char timeDelayBuf[255];
char messageBuf[255];
char msMultiplayerBuf[255];

HWND hwndMain, hwndTimeEdit, hwndMessageEdit, hwndApplyButton,
				hwndStatic1, hwndStatic2, hwndComboBox, hwndCloseButton;
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ButtonProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK ComboProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
void timerCallback();
WNDPROC OldButtonProc, OldComboProc;

/*  Make the class name into a global variable  */
char szClassName[ ] = "WindowsApp";

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)

{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = (HICON)LoadImage(hThisInstance, MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 32, 32, LR_SHARED);
    wincl.hIconSm = (HICON)LoadImage(hThisInstance, MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 16, 16, LR_SHARED);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwndMain = CreateWindowEx (
            WS_EX_CLIENTEDGE,
                 /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "Reminder",       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           543,                 /* The programs width */
           84,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

   hwndStatic1 = CreateWindowEx(WS_EX_CLIENTEDGE,
							"STATIC",
							"Remind me every",
							WS_BORDER|WS_VISIBLE|WS_CHILD|ES_CENTER,
							0,25+OFFSET,150,23,
							hwndMain,
							NULL,
							hThisInstance,
							NULL);

   hwndStatic2 = CreateWindowEx(WS_EX_CLIENTEDGE,
							"STATIC",
							"Message",
							WS_BORDER|WS_VISIBLE|WS_CHILD|ES_CENTER,
							0,2,50,23,
							hwndMain,
							NULL,
							hThisInstance,
							NULL);

	hwndMessageEdit = CreateWindowEx(WS_EX_CLIENTEDGE,
							"EDIT",
							"Time elapsed",
							WS_BORDER|WS_VISIBLE|WS_CHILD|ES_AUTOVSCROLL,
							52,2,478,23,
							hwndMain,
							NULL,
							hThisInstance,
							NULL);

	// Create Edit window
	hwndTimeEdit = CreateWindowEx(WS_EX_CLIENTEDGE,
							"EDIT",
							"4",
							WS_BORDER|WS_VISIBLE|WS_CHILD|ES_AUTOVSCROLL,
							152,25+OFFSET,50,23,
							hwndMain,
							NULL,
							hThisInstance,
							NULL);

	// Create ListBox
	hwndComboBox = CreateWindowEx(WS_EX_CLIENTEDGE,
							"COMBOBOX",
							NULL,
							WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | CBS_AUTOHSCROLL ,
							204,26+OFFSET,100,150,
							hwndMain,
							NULL,
							hThisInstance,
							NULL);
	SendMessage(hwndComboBox, CB_ADDSTRING, 0, (LPARAM) "secs");
	SendMessage(hwndComboBox, CB_ADDSTRING, 0, (LPARAM) "mins");
	SendMessage(hwndComboBox, CB_ADDSTRING, 0, (LPARAM) "hours");
	SendMessage(hwndComboBox, CB_SETCURSEL, 2, 0);
							
	// Create Button
	hwndApplyButton = CreateWindowEx(0,
								"BUTTON",
								"&Start",
								WS_VISIBLE|WS_CHILD|BS_PUSHLIKE,
								310,26+OFFSET,100,20,
								hwndMain,
								NULL,
								hThisInstance,
								NULL);
								
  hwndCloseButton = CreateWindowEx(0,
								"BUTTON",
								"&Close",
								WS_VISIBLE|WS_CHILD|BS_PUSHLIKE,
								430,26+OFFSET,100,20,
								hwndMain,
								NULL,
								hThisInstance,
								NULL);

	// SubClass
	OldButtonProc = (WNDPROC) SetWindowLong (hwndApplyButton,GWL_WNDPROC,(LONG)ButtonProc);
	OldComboProc = (WNDPROC) SetWindowLong (hwndComboBox,GWL_WNDPROC,(LONG)ComboProc);


	HGDIOBJ hObj = GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(hwndApplyButton, WM_SETFONT,(WPARAM)hObj, true);
	SendMessage(hwndCloseButton, WM_SETFONT,(WPARAM)hObj, true);
	SendMessage(hwndComboBox, WM_SETFONT,(WPARAM)hObj, true);
	SendMessage(hwndTimeEdit, WM_SETFONT,(WPARAM)hObj, true);
	SendMessage(hwndMessageEdit, WM_SETFONT,(WPARAM)hObj, true);
	SendMessage(hwndStatic1, WM_SETFONT,(WPARAM)hObj, true);
	SendMessage(hwndStatic2, WM_SETFONT,(WPARAM)hObj, true);




    /* Make the window visible on the screen */
    ShowWindow (hwndMain, nCmdShow);
    timer.setCallbackFunction(&timerCallback);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {

    		case WM_COMMAND:
        	   if((HWND)lParam == hwndComboBox)
        	   		switch (HIWORD(wParam)) {
                		case CBN_CLOSEUP:
                				CallWindowProc(OldComboProc, hwnd, message, wParam, lParam);
/*               				LRESULT item;
                				item = SendMessage(hwndComboBox, CB_GETCURSEL, 0, 0);
                				char msg[255];
            				    sprintf(msg,"item %d", item);
            				    MessageBox(NULL, msg, "", 0);
*/
            				    break;
            				 //case: CBN_
                    default:
                        DefWindowProc (hwnd, message, wParam, lParam);
                        break;
              }
              if ((HWND) lParam == hwndApplyButton) {
              	//  case BN_CLICKED:
                  	DefWindowProc(hwnd, message, wParam, lParam);
                  	char *cmd;
                  	
                  	if (toggleOn) {
                  		cmd = "&Start\0";
          		      } else {
          		         cmd = "&Stop\0";
                    }
                    EnableWindow(hwndComboBox, toggleOn);
                    EnableWindow(hwndTimeEdit, toggleOn);
                    EnableWindow(hwndMessageEdit, toggleOn);
                    SendMessage(hwndApplyButton, WM_SETTEXT, 0 , (LPARAM)cmd);
                    toggleOn = !toggleOn;
                    
                    if (toggleOn) {
                    		SendMessage(hwndTimeEdit, WM_GETTEXT, 256 , (LPARAM)timeDelayBuf);
                    		SendMessage(hwndMessageEdit, WM_GETTEXT, 256 , (LPARAM)messageBuf);
                    		
                    		timeDelay = atof(timeDelayBuf);
                    		LRESULT item;
                				item = SendMessage(hwndComboBox, CB_GETCURSEL, 0, 0);
                				msMultiplayer = 1;
                    		switch (item) {
                        	case 2: // hour to min
                        		msMultiplayer *= 60;
                          case 1: // min to sec
                          	msMultiplayer *= 60;
                          case 0: //sec to ms
                          	msMultiplayer *= 1000;
                          	break;
                          default:
                          	msMultiplayer *= 10000;
                        }
                        
/*                      char msg[255];
            				    sprintf(msg,"delay %d", timeDelay*msMultiplayer);
            				    MessageBox(NULL, msg, "", 0);
*/                      if (timeDelay > 0.0f) {
		            					timer.start((long)(timeDelay*msMultiplayer));
		            			  } else {
                     			MessageBox(NULL,"Wrong time delay specified", "Error",MB_ICONSTOP );
                        	EnableWindow(hwndComboBox, toggleOn);
                          EnableWindow(hwndTimeEdit, toggleOn);
                          EnableWindow(hwndMessageEdit, toggleOn);
                          toggleOn = !toggleOn;
                          cmd = "&Start\0";
                          SendMessage(hwndApplyButton, WM_SETTEXT, 0 , (LPARAM)cmd);
                        }
                    } else {
                     	timer.stop();
                    }
                    break;
               }
               if ((HWND) lParam == hwndCloseButton) {
               		PostQuitMessage (0);
               		break;
               }
               break;
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

LRESULT CALLBACK ButtonProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
	HDC hdc;
	HPEN hpen;
	HBRUSH hbr;
	HGDIOBJ oldpen, oldbr;
	LOGBRUSH logbrush;

	switch(msg)
	{
	case WM_PAINT:
		// Draw Original Button
		CallWindowProc(OldButtonProc, hwnd, msg, wParam, lParam);

		// Now we add a Red Border

		GetClientRect(hwnd, &rc);
		// Get the dc
		hdc = GetDC(hwnd);
		// Create Pen
		hpen = CreatePen(PS_SOLID, 4, RGB(255,0,0)); // Solid, 4px width, red color
		// Make this pen active by selecting it into dc
		oldpen = SelectObject(hdc, (HGDIOBJ) hpen);

		logbrush.lbStyle = BS_HOLLOW; // Insides of solid objects are not painted
		// Create Brush
		hbr = CreateBrushIndirect(&logbrush);
		// Make the brush active by selecting it into dc
		oldbr = SelectObject(hdc, (HGDIOBJ) hbr);

		//Draw the rectangle
		Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

		// Always remember to clean up !
		SelectObject(hdc, oldbr);
		SelectObject(hdc, oldpen);
		// Delete objects
		DeleteObject(hbr);
		DeleteObject(hpen);

		ReleaseDC(hwnd, hdc);

		break;
	default:
		return CallWindowProc(OldButtonProc, hwnd, msg, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK ComboProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {
	RECT rc;
	HDC hdc;
	HPEN hpen;
	HBRUSH hbr;
	HGDIOBJ oldpen, oldbr;
	LOGBRUSH logbrush;

	switch(msg)
	{
		case WM_PAINT:
			// Draw Original combo
			CallWindowProc(OldComboProc, hwnd, msg, wParam, lParam);
		break;
	default:

		return CallWindowProc(OldComboProc, hwnd, msg, wParam, lParam);
	}
	return 0;
}

DWORD WINAPI MagBox( LPVOID lpParam );
static long counter =0;

void timerCallback() {
  DWORD iID;
	HANDLE hThread;
 	hThread = CreateThread(NULL, 0, MagBox, NULL ,0,&iID);
  
  CloseHandle(hThread);
  hThread = NULL;
}

DWORD WINAPI MagBox( LPVOID lpParam ) {
	char msg[255];
	SYSTEMTIME st;
	GetSystemTime(&st);
	
	counter++;
  sprintf(msg,"Alert %d - %02d:%02d:%02d UTC", counter, st.wHour,st.wMinute,st.wSecond);
	MessageBox(NULL,messageBuf, msg,MB_ICONEXCLAMATION );
}

