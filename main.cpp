//-------------------Imports-------------------
#include <windows.h>
#include <cmath>
#include <fstream>
#include <utility>
#include <string>
#include <iostream>

//-------------------Very useful macro-------------------
#define UNUSED(x) (void)(x)


//-------------------Some functions-------------------
void LoadPrimals();
void StorePrimals();

//-------------------SetWindowText wrappers-------------------
void PrintL( int number );
void PrintR( int number );
void ClearL();
void ClearR();

//-------------------Main factorizing function-------------------
void factorize(int number);

//-------------------Window size-------------------
short int wWidth  = 400;
short int wHeight = 700;

//-------------------Controls-------------------
HWND hWnd;
HWND hInput;
HWND hOutputL;
HWND hOutputSeparator;
HWND hOutputR;
HWND hButton;
HBRUSH hSeparatorBrush;
HBRUSH hOutputBrush;

//-------------------Output text buffers-------------------
std::string BufferL;
std::string BufferR;


class list{
	//very awfull, but working, list implementation
	public:
	struct list_element{
		list_element *next;
		list_element *prev;
		int value;
	};
	private:
	list_element *_start, *_end;
	
	size_t _size;
	bool _changed;
	public:
	
	list(){
		this->_start = NULL;
		this->_end  = NULL;
		this->_size  = 0;
		this->_changed = false;
	}
	
	size_t size(){
		return this->_size;
	}
	
	void push( int value ){
		list_element *new_element = new list_element{ this->_start, this->_end, value };
		if( this->_size ){
			this->_start->prev = new_element;
			this->_end->next  = new_element;
			this->_end = new_element;
		}
		else{
			this->_start = new_element;
			this->_start->next = this->_start;
			this->_start->prev = this->_start;
			this->_end = this->_start;
		}
		
		this->_size++;
		this->_changed = true;
	}
	
	void remove( list_element *to_delete ){
		to_delete->prev->next = to_delete->next;
		to_delete->next->prev = to_delete->prev;
		delete to_delete;
		this->_size--;
		this->_changed = true;
	}
	
	list_element* begin(){
		return this->_start;
	}
	
	list_element* end(){
		return this->_end;
	}
	
	bool has_changed(){
		return this->_changed;
	}
	
	void set_unchanged(){
		this->_changed=false;
	}
	
} primals;


LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	switch( uMsg ){
		case WM_CREATE:
			break;
		
		case WM_DESTROY:
			PostQuitMessage( 0 );
			break;
			
		case WM_CTLCOLORSTATIC:
			if( ( HWND ) lParam == hOutputSeparator ){
				return ( LRESULT ) hSeparatorBrush;
			}
			else if( ( HWND ) lParam == hOutputL || ( HWND ) lParam == hOutputR ){
				return ( LRESULT ) hOutputBrush;
			}
			break;
			
		case WM_SIZE:
			wWidth  = LOWORD( lParam );
			wHeight = HIWORD( lParam );
			MoveWindow( hButton, wWidth/4 + ( wWidth/2 - 50 ), 50, 50, 25, true );
			MoveWindow( hInput,  wWidth/4, 50, wWidth/2 - 50, 25, true );//25 px->3 signs
			MoveWindow( hOutputL, 20, 85, ( wWidth - 46 )/2, wHeight - 110, true );
			MoveWindow( hOutputR, 20 + ( wWidth - 36 )/2, 85, wWidth/2 - 20, wHeight - 110, true );
			MoveWindow( hOutputSeparator, 16 + ( wWidth - 36 )/2, 90, 2, wHeight - 110, true );
			break;
			
		case WM_COMMAND:
			if( ( HWND ) lParam == hButton ){
				
				int length = GetWindowTextLength( hInput );
				char* buf = (char*)malloc(length+1);
				GetWindowText( hInput, buf, length+1 );
					
				if( atoi( buf ) != 0 )
					factorize( atoi( buf ) );
				else
					#ifdef POLISH
					MessageBoxW( hWnd, L"Wpisana wartość nie może być zerem!", L"Błąd", MB_OK | MB_ICONWARNING );
					#else
					MessageBoxW( hWnd, L"Entered value can not be zero!", L"Error", MB_OK | MB_ICONWARNING );
					#endif
				SetWindowText( hInput, "" );
			}
			break;
			
		default:
			return ( DefWindowProc( hWnd, uMsg, wParam, lParam ) );
	}
	return ( 0L ); 
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	UNUSED(hPrevInstance);
	UNUSED(lpCmdLine);
	UNUSED(nCmdShow);
	LoadPrimals();
	MSG   msg;
    WNDCLASS wndclass;
	TCHAR lpszClassName[] = TEXT("factorization");
	#ifdef POLISH
	LPCWSTR WindowCaptionW = L"Rozkład liczb na czynniki pierwsze";
	#else
	LPCWSTR WindowCaptionW = L"Factorization";
	#endif
	
    wndclass.style  = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = MainWndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance  = hInstance;
    wndclass.hIcon  = ( HICON ) LoadImage( NULL, "icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED );
    wndclass.hCursor  = LoadCursor( NULL, IDC_ARROW );
    wndclass.hbrBackground = ( HBRUSH ) GetStockObject( WHITE_BRUSH );
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = lpszClassName;
	
	 if( RegisterClass( &wndclass ) == 0 )
        return FALSE;
	
	TCHAR WindowCaption[ 40 ];
	WideCharToMultiByte( CP_ACP, 0, WindowCaptionW, -1, WindowCaption, 100, NULL, NULL );
	
	hWnd = CreateWindow(
        lpszClassName, WindowCaption,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        100, 100, wWidth, wHeight,
        NULL, NULL, hInstance, NULL );
	
	hButton = CreateWindowEx( 0, "BUTTON", "ok",
	WS_CHILD | WS_VISIBLE, wWidth/2 + 70, 30, 50, 30,
	hWnd, NULL, hInstance, NULL );
	
	hSeparatorBrush=CreateSolidBrush( RGB( 0, 0, 0 ) );
	hOutputBrush=CreateSolidBrush( RGB( 255, 255, 255 ) );
	
	
	hOutputL=CreateWindowEx( 0, "STATIC", NULL,
	WS_CHILD | WS_VISIBLE | ES_RIGHT, 20, 100, ( wWidth - 46 )/2, wHeight - 120,
	hWnd, NULL, hInstance, NULL );
	
	hOutputSeparator=CreateWindowEx( 0, "STATIC", NULL,
	WS_CHILD | WS_VISIBLE, 16 + ( wWidth-36 )/2, 100, 2, wHeight - 120,
	hWnd, NULL, hInstance, NULL);
	
	hOutputR=CreateWindowEx( 0, "STATIC", NULL,
	WS_CHILD | WS_VISIBLE, 26 + ( wWidth - 46 )/2, 100, ( wWidth - 46 )/2, wHeight - 120,
	hWnd,NULL,hInstance,NULL );
	
	hInput = CreateWindowEx( 0, "EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, wWidth/2-90, 50, 150, 30,
hWnd, NULL, hInstance, NULL );
	
	
	
	ShowWindow( hWnd, SW_SHOW );
    UpdateWindow( hWnd );
	
	
	while( GetMessage( &msg, NULL, 0, 0 ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
	
	
    return msg.wParam;
}

void LoadPrimals(){
	std::fstream primals_f;
	primals_f.open( "primals.txt", std::ios::in );
	
	if( primals_f.good() ){
		std::string primal;
		while( getline( primals_f, primal ) )
			primals.push( atoi( primal.c_str() ) );
	}
	primals_f.close();
	if( primals.size() == 0 ){
		primals.push( 2 );
		primals.push( 3 );
	}
	else if( primals.size() == 1 )
		primals.push( 3 );
	primals.set_unchanged();
}

void StorePrimals(){
	std::fstream primals_f;
	primals_f.open( "primals.txt", std::ios::out|std::ios::trunc );
	if( primals_f.good() )
		for( std::pair<list::list_element*, size_t> i( primals.begin(),(unsigned int)  0 ); i.second < primals.size(); i.first = i.first->next, i.second++ )
			primals_f<<i.first->value<<std::endl;
	primals_f.close();
}

void ClearL(){
	BufferL="";
	SetWindowText( hOutputL, BufferL.c_str());
}

void ClearR(){
	BufferR="";
	SetWindowText( hOutputR, BufferR.c_str());
}

void PrintL( int number ){
	BufferL += std::to_string(number) + "\n";
	SetWindowText( hOutputL, BufferL.c_str());
}

void PrintR( int number ){
	BufferR += std::to_string(number) + "\n";
	SetWindowText( hOutputR, BufferR.c_str());
}

void factorize( int number ){
	ClearL();
	ClearR();
	
	//generating list of primal numbers using sieve of Eratosthenes
	auto end = primals.end();
	size_t size  = primals.size();
	for( int i = end->value + 2; i <= number; i+=2 )
		primals.push( i );
	
	if( size != primals.size() ){
		auto i = primals.begin()->next;
		auto j = end;
		
		for( size_t counter_i = 0;  counter_i < primals.size(); counter_i++  ){
			if( i->value > end->value )
				j = i;
			else
				j = end;
			for( size_t counter_j = size; counter_j < primals.size(); counter_j++ ){
				j = j->next;
				if( j->value != i->value && (j->value % i->value) == 0 ){
					j = j->next;
					primals.remove( j->prev );
				}
			}
				
			if( i->value > ( int ) std::ceil( std::sqrt( number ) ) )
				break;
			i = i->next;
		}
	}
	
	//main factorization loop
	auto primal = primals.begin();
	for( size_t i = 0; i < primals.size(); i++ ){
		if( number == 1 )
			break;
		if( number % primal->value == 0 ){
			PrintL( number );
			PrintR( primal->value );
			number /= primal->value;
		}
		else
			primal = primal->next;
	}
	PrintL( 1 );
	if( primals.has_changed())
		StorePrimals();
}