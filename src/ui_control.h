

#ifndef __SGS_UI_CONTROL__
#define __SGS_UI_CONTROL__


#include "ui_utils.h"

#include <float.h>


#define UI_MaxValue FLT_MAX

#define UI_NO_ID 0xffffffff

#define Anchor_Top    0x01
#define Anchor_Bottom 0x02
#define Anchor_Left   0x04
#define Anchor_Right  0x08
#define Anchor_Hor   (Anchor_Left | Anchor_Right)
#define Anchor_Vert  (Anchor_Top | Anchor_Bottom)
#define Anchor_All   (Anchor_Hor | Anchor_Vert)

#define EV_Paint      1
#define EV_Layout     2
#define EV_ChgTheme   3
#define EV_KeyDown    10
#define EV_KeyUp      11
#define EV_Char       12
#define EV_FocusEnter 13
#define EV_FocusLeave 14
#define EV_NeedFocus  15
#define EV_ButtonDown 20
#define EV_ButtonUp   21
#define EV_MouseMove  22
#define EV_MouseEnter 23
#define EV_MouseLeave 24
#define EV_MouseWheel 25
#define EV_Attach     30
#define EV_Detach     31
#define EV_HitTest    32
#define EV_AddChild   33
#define EV_RemChild   34

#define Mouse_ButtonL 0
#define Mouse_ButtonR 1
#define Mouse_ButtonM 2
#define Mouse_Button_Count 3

#define Key_Left      1  // shift makes selection stick
#define Key_Right     2  // ^
#define Key_Up        3  // ^
#define Key_Down      4  // ^
#define Key_DelLeft   5
#define Key_DelRight  6
#define Key_Tab       7  // shift makes focus move backwards
#define Key_Cut       8
#define Key_Copy      9
#define Key_Paste     10
#define Key_Undo      11
#define Key_Redo      12
#define Key_SelectAll 13
#define Key_PageUp    14
#define Key_PageDown  15

#define Hit_Client    2
#define Hit_NonClient 1
#define Hit_None      0

#define UI_Cursor_Arrow      0
#define UI_Cursor_Text       1
#define UI_Cursor_Wait       2
#define UI_Cursor_Crosshair  3
#define UI_Cursor_WaitArrow  4
#define UI_Cursor_SizeNWSE   5
#define UI_Cursor_SizeNESW   6
#define UI_Cursor_SizeWE     7
#define UI_Cursor_SizeNS     8
#define UI_Cursor_SizeAll    9
#define UI_Cursor_No         10
#define UI_Cursor_Hand       11

#define KeyMod_Filter 0x0FF
#define KeyMod_Shift  0x100


struct UIRect
{
	float x0, y0, x1, y1;
};
typedef std::vector< UIRect > UIRectArray;


struct UITimerData
{
	int64_t id;
	float interval;
	float timeout;
	sgsVariable func;
};

typedef std::map< int64_t, UITimerData > UITimerMap;


struct UIEvent
{
	SGS_OBJECT_LITE;
	
	UIEvent() : type(0), key(0), button(0), uchar(0), x(0), y(0), rx(0), ry(0){}
	
	SGS_PROPERTY int type;
	SGS_PROPERTY int key;
	SGS_PROPERTY int button;
	SGS_PROPERTY int uchar;
	SGS_PROPERTY float x;
	SGS_PROPERTY float y;
	SGS_PROPERTY float rx;
	SGS_PROPERTY float ry;
};


struct UIStyle /* style storage */
{
	SGS_OBJECT_LITE;
	
	SGS_PROPERTY sgsMaybe<float> x;
	SGS_PROPERTY sgsMaybe<float> y;
	SGS_PROPERTY sgsMaybe<float> width;
	SGS_PROPERTY sgsMaybe<float> height;
	SGS_PROPERTY sgsMaybe<float> q0x;
	SGS_PROPERTY sgsMaybe<float> q0y;
	SGS_PROPERTY sgsMaybe<float> q1x;
	SGS_PROPERTY sgsMaybe<float> q1y;
	SGS_PROPERTY sgsMaybe<float> nc_top;
	SGS_PROPERTY sgsMaybe<float> nc_left;
	SGS_PROPERTY sgsMaybe<float> nc_right;
	SGS_PROPERTY sgsMaybe<float> nc_bottom;
	SGS_PROPERTY sgsMaybe<bool>  visible;
	SGS_PROPERTY sgsMaybe<int>   index;
	SGS_PROPERTY sgsMaybe<bool>  topmost;
	SGS_PROPERTY sgsMaybe<float> minWidth;
	SGS_PROPERTY sgsMaybe<float> maxWidth;
	SGS_PROPERTY sgsMaybe<float> minHeight;
	SGS_PROPERTY sgsMaybe<float> maxHeight;
	SGS_PROPERTY sgsMaybe<float> marginLeft;
	SGS_PROPERTY sgsMaybe<float> marginRight;
	SGS_PROPERTY sgsMaybe<float> marginTop;
	SGS_PROPERTY sgsMaybe<float> marginBottom;
	SGS_PROPERTY sgsMaybe<float> paddingLeft;
	SGS_PROPERTY sgsMaybe<float> paddingRight;
	SGS_PROPERTY sgsMaybe<float> paddingTop;
	SGS_PROPERTY sgsMaybe<float> paddingBottom;
	SGS_PROPERTY sgsVariable     cursor;
	SGS_PROPERTY sgsString       font;
	SGS_PROPERTY sgsMaybe<float> fontSize;
	SGS_PROPERTY sgsVariable     renderfunc;
	
	void gcmark(){ cursor.gcmark(); renderfunc.gcmark(); }
};
SGS_DEFAULT_LITE_OBJECT_INTERFACE( UIStyle );

struct UIStyleCache /* computed style cache */
{
	float x;
	float y;
	float width;
	float height;
	float q0x;
	float q0y;
	float q1x;
	float q1y;
	float nc_top;
	float nc_left;
	float nc_right;
	float nc_bottom;
	bool  visible;
	int   index;
	bool  topmost;
	float minWidth;
	float maxWidth;
	float minHeight;
	float maxHeight;
	float marginLeft;
	float marginRight;
	float marginTop;
	float marginBottom;
	float paddingLeft;
	float paddingRight;
	float paddingTop;
	float paddingBottom;
	sgsVariable cursor;
	sgsString   font;
	float       fontSize;
	sgsVariable renderfunc;
};

struct UIStyleSelector
{
	struct Fragment
	{
		enum Type
		{
			T_None = 0,
			T_MoveOn, // all done with current control, can move on to next
			T_ReqNext, // require that the element to be matched is next (directly related) in line of parents/children (>)
			T_MatchAny, // doesn't matter what's there - as long as there's something
			T_MatchType, // first word matches the type (button,autolayout,..) of the control, if any is given
			// following words, prefixed by dot, specify classes to match
			T_MatchClassExact, // .class
			T_MatchClassPartBegin, // .^class
			T_MatchClassPartEnd, // .$class
			T_MatchClassPart, // .~class
			// following words, prefixed by 'at' (@), specify the exact name attribute to expect
			T_MatchNameExact, // @name
			T_MatchNamePartBegin, // @^name
			T_MatchNamePartEnd, // @$name
			T_MatchNamePart, // @~name
		};
		
		Type type;
		const char* start;
		const char* end;
	};
	
	typedef std::vector< Fragment > FragmentArray;
	
	sgsString selector;
	FragmentArray fragments;
	int numnext;
	int numtypes;
	int numclasses;
	int numnames;
};
typedef std::vector< UIStyleSelector > StyleSelArray;

struct UIStyleRule
{
	typedef sgsHandle< UIStyleRule > Handle;
	typedef std::vector< Handle > HandleArray;
	
	SGS_OBJECT;
	
	SGS_PROPERTY UIStyle style;
	StyleSelArray selectors;
	
	SGS_METHOD SGS_MULTRET addSelector( sgsString str );
	SGS_METHOD bool checkMatch( sgsHandle< struct UIControl > ctrl );
};

struct UIStyleSheet
{
	typedef sgsHandle< UIStyleSheet > Handle;
	
	SGS_OBJECT;
	
	SGS_IFUNC(GCMARK) int sgs_gcmark( SGS_CTX, sgs_VarObj* obj );
	
	UIStyleRule::HandleArray rules;
	
	SGS_METHOD void addRule( UIStyleRule::Handle rule ){ if( rule.object && !VFIND( rules, rule ) ) rules.push_back( rule ); }
};
typedef std::vector< UIStyleSheet::Handle > StyleSheetArray;


const char* UI_ParseSelector( UIStyleSelector* sel, sgsString str );
bool UI_SelectorTestControl( const UIStyleSelector* sel, struct UIControl* ctrl );
int UI_CompareSelectors( const UIStyleSelector* sel1, const UIStyleSelector* sel2 );
void UI_StyleMerge( UIStyle* style, UIStyle* add );
void UI_ToStyleCache( UIStyleCache* cache, UIStyle* style );

bool UI_TxMatchExact( const char* stfrom, const char* stto, const char* tgt, size_t tgtsize );
bool UI_TxMatchPartBegin( const char* stfrom, const char* stto, const char* tgt, size_t tgtsize );
bool UI_TxMatchPartEnd( const char* stfrom, const char* stto, const char* tgt, size_t tgtsize );
bool UI_TxMatchPart( const char* stfrom, const char* stto, const char* tgt, size_t tgtsize );

struct UIFilteredStyle
{
	UIStyleRule* rule;
	size_t which_sel; // selector number
};
typedef std::vector< UIFilteredStyle > UIFilteredStyleArray;


struct UIControl;
struct UIFrame
{
	typedef sgsHandle< UIFrame > Handle;
	typedef std::vector< UIControl* > CtrlPtrArray;
	
	SGS_OBJECT;
	
	UIFrame();
	~UIFrame();
	
	SGS_METHOD sgsHandle< UIControl > createControl( sgsString type );
	
	SGS_METHOD void event( UIEvent* e );
	SGS_METHOD void render();
	UIControl* _getControlAtPosition( float x, float y );
	SGS_METHOD void handleMouseMove();
	SGS_METHOD void setFocus( UIControl* ctrl );
	
	// event generation shortcuts
	SGS_METHOD void doMouseMove( float x, float y );
	SGS_METHOD void doMouseButton( int btn, bool down );
	SGS_METHOD void doMouseWheel( float x, float y );
	SGS_METHOD void doKeyPress( int key, bool down );
	SGS_METHOD void doPutChar( int chr );
	
	// timers
	SGS_METHOD int64_t setTimeout( float t, sgsVariable func ){ return _setTimer( t, func, true ); }
	SGS_METHOD int64_t setInterval( float t, sgsVariable func ){ return _setTimer( t, func, false ); }
	SGS_METHOD void clearTimeout( int64_t id ){ _clearTimer( id ); }
	SGS_METHOD void clearInterval( int64_t id ){ _clearTimer( id ); }
	int64_t _setTimer( float t, sgsVariable func, bool one_shot );
	void _clearTimer( int64_t id );
	SGS_METHOD void processTimers( float t );
	
	// scissor rectangle
	SGS_METHOD bool pushScissorRect( float x0, float y0, float x1, float y1 );
	SGS_METHOD bool pushScissorRectUnclipped( float x0, float y0, float x1, float y1 );
	SGS_METHOD bool popScissorRect();
	SGS_METHOD int getScissorRectCount(){ return m_scissorRects.size(); }
	void _applyScissorState();
	
	SGS_IFUNC(GCMARK) int sgs_gcmark( SGS_CTX, sgs_VarObj* obj );
	
	// styling
	SGS_METHOD void addStyleSheet( UIStyleSheet::Handle sheet );
	SGS_METHOD void removeStyleSheet( UIStyleSheet::Handle sheet );
	SGS_METHOD sgsVariable getStyleSheets();
	SGS_METHOD UIStyleSheet::Handle getStyleSheet( size_t i ){ if( i >= m_styleSheets.size() ) return UIStyleSheet::Handle(); return m_styleSheets[ i ]; }
	SGS_METHOD size_t getStyleSheetCount(){ return m_styleSheets.size(); }
	void _updateStyles();
	
	// info retrieval
	SGS_METHOD float getClickOffsetX( int button ){ if( button < 0 || button >= Mouse_Button_Count ) return 0; return m_clickoffsets[ button ][0]; }
	SGS_METHOD float getClickOffsetY( int button ){ if( button < 0 || button >= Mouse_Button_Count ) return 0; return m_clickoffsets[ button ][1]; }
	SGS_METHOD sgsHandle< UIControl > getHoverControl();
	SGS_METHOD sgsHandle< UIControl > getFocusControl();
	SGS_METHOD sgsHandle< UIControl > getControlUnderCursor();
	SGS_METHOD sgsHandle< UIControl > getControlUnderPoint( float x, float y );
	SGS_METHOD bool isControlUnderCursor( const sgsHandle< UIControl >& ctrl );
	SGS_METHOD bool isControlUnderPoint( const sgsHandle< UIControl >& ctrl, float x, float y );
	
	SGS_PROPERTY sgsVariable scissor_func; /* no arguments (all NULL) -> clear, 4 arguments (int) -> set */
	SGS_PROPERTY sgsVariable clipboard_func; /* no arguments -> get; 1 argument (string) -> set */
	
	/*	cursor_func
		argument: cursor variable from control (unknown type)
		function must support the following values:
		- null: default / do not change (cursor outside UI)
		- boolean `false`: no cursor
		- boolean `true`: default cursor
		- integer UI_Cursor_ constants
	*/
	SGS_PROPERTY sgsVariable cursor_func;
	
	/*	font_func
		arguments: font name, font size
		- null name returns the default font
		should return object capable of being used by the theme
	*/
	SGS_PROPERTY sgsVariable font_func;
	
	/*	theme
		dict containing data about theme
		the convention:
		- <type>_renderfunc - rendering function for the specified control type
		- <type>_<other> - misc. data for the specified control type
	*/
	SGS_PROPERTY_FUNC( READ WRITE WRITE_CALLBACK updateTheme ) sgsVariable theme;
	
	
	SGS_PROPERTY_FUNC( READ WRITE WRITE_CALLBACK updateLayout ) float x;
	SGS_PROPERTY_FUNC( READ WRITE WRITE_CALLBACK updateLayout ) float y;
	SGS_PROPERTY_FUNC( READ WRITE WRITE_CALLBACK updateLayout ) float width;
	SGS_PROPERTY_FUNC( READ WRITE WRITE_CALLBACK updateLayout ) float height;
	SGS_PROPERTY READ sgsHandle< UIControl > root;
	
	SGS_PROPERTY READ float mouseX;
	SGS_PROPERTY READ float mouseY;
	
	void updateLayout();
	void updateTheme();
	void forceUpdateCursor( UIControl* ctrl );
	void preRemoveControl( UIControl* ctrl );
	
	UIControl* m_hover;
	UIControl* m_focus;
	UIControl* m_clicktargets[ Mouse_Button_Count ];
	float m_clickoffsets[ Mouse_Button_Count ][2];
	UITimerMap m_timers;
	int64_t m_timerAutoID;
	IDGen m_controlIDGen;
	UIRectArray m_scissorRects;
	CtrlPtrArray m_animatedControls;
	StyleSheetArray m_styleSheets;
	
};


struct UIControl
{
	typedef sgsHandle< UIControl > Handle;
	typedef std::vector< Handle > HandleArray;
	
	static Handle CreateHandle( UIControl* ctrl )
	{
		if( ctrl )
			return Handle( ctrl->m_sgsObject, ctrl->C );
		return Handle();
	}
	
	
	// Animation tools
	struct Animation
	{
		sgsVariable prevState;
		sgsVariable currState;
		float time, end;
		sgsVariable func; /* args: v0, v1, q, tend;  ret = interpolated */
		sgsVariable oncomplete; /* this = control */
		
		void gcmark(){ prevState.gcmark(); currState.gcmark(); func.gcmark(); oncomplete.gcmark(); }
	};
	typedef std::vector< Animation > AnimArray;
	
	
	SGS_OBJECT;
	
	UIControl();
	~UIControl();
	
	int niEvent( UIEvent* event );
	void niBubblingEvent( UIEvent* e );
	void niRender();
	SGS_METHOD void updateLayout();
	SGS_METHOD void updateTheme();
	void updateThemeRecursive();
	void updateCursor();
	void updateFont();
	
	SGS_METHOD bool addChild( UIControl::Handle ch );
	SGS_METHOD bool removeChild( UIControl::Handle ch );
	SGS_METHOD bool removeAllChildren();
	SGS_METHOD UIControl::Handle findChild( sgsString name );
	SGS_METHOD sgsVariable children( bool nonclient );
	SGS_METHOD void sortChildren();
	SGS_METHOD void sortSiblings();
	SGS_METHOD void setAnchorMode( int mode );
	SGS_METHOD void setAnchorRect( float x0, float y0, float x1, float y1 );
	
	SGS_METHOD bool bindEvent( sgsString name, sgsVariable callable );
	SGS_METHOD bool unbindEvent( sgsString name, sgsVariable callable );
	SGS_METHOD bool callEvent( sgsString name, UIEvent* e );
	
	SGS_METHOD UIControl::Handle animate( sgsVariable state, float length, sgsVariable func, sgsVariable oncomplete );
	SGS_METHOD UIControl::Handle stop( bool nofinish ); /* skip + dequeue */
	SGS_METHOD UIControl::Handle dequeue(); /* remove all except current (time>0) */
	SGS_METHOD UIControl::Handle skip( bool nofinish ); /* remove current */
	SGS_METHOD int queueSize(){ return m_animQueue.size(); }
	void _advanceAnimation( float dt );
	void _applyCurAnimState();
	void _finishCurAnim();
	void _startCurAnim();
	
	SGS_IFUNC(GETINDEX) int sgs_getindex( SGS_CTX, sgs_VarObj* obj, sgs_Variable* key, int isprop );
	SGS_IFUNC(GCMARK) int sgs_gcmark( SGS_CTX, sgs_VarObj* obj );
	
	/// PRIMARY DATA
	SGS_PROPERTY READ uint32_t id;
	SGS_PROPERTY sgsString name;
	SGS_PROPERTY sgsString caption;
	SGS_PROPERTY sgsString type;
	SGS_PROPERTY READ Handle parent;
	SGS_PROPERTY READ UIFrame::Handle frame;
	
	sgsString classes;
	SGS_METHOD sgsString getClasses(){ return classes; }
	SGS_METHOD void setClasses( const sgsString& ss ){ classes = ss; }
	SGS_METHOD bool addClass( const sgsString& ss ){ return addClass( ss.c_str(), ss.size() ); }
	SGS_METHOD bool removeClass( const sgsString& ss ){ return removeClass( ss.c_str(), ss.size() ); }
	SGS_METHOD bool hasClass( const sgsString& ss ){ return hasClass( ss.c_str(), ss.size() ); }
	void _setClasses3( const char* str1, size_t size1, const char* str2, size_t size2, const char* str3, size_t size3 );
	bool addClass( const char* str, size_t size );
	bool removeClass( const char* str, size_t size );
	bool hasClass( const char* str, size_t size );
	bool hasClassPartBegin( const char* str, size_t size );
	bool hasClassPartEnd( const char* str, size_t size );
	bool hasClassPart( const char* str, size_t size );
	size_t _findClassAt( const char* str, size_t size, int checksides = 1|2 );
	static void _trimClass( const char** str, size_t* size );
	
	/// STYLES
	UIStyleCache computedStyle; // active style data (after merging rule style and local override, on applying changes)
	UIStyle filteredStyle; // calculated from style rules
	UIStyle style; // local override
	void _refilterStyles( UIFilteredStyleArray* styles );
	void _remergeStyle();
	void _applyStyle( const UIStyleCache& nsc );
	
#define UIC_DEFINE_ACCESSORS( ty, nm ) \
	ty get_##nm() const { return computedStyle.nm; } \
	void set_##nm( ty nm ){ style.nm.set( nm ); _remergeStyle(); }
#define UIC_DEFINE_ACCESSORS2( ty, nm ) \
	ty get_##nm() const { return computedStyle.nm; } \
	void set_##nm( ty nm ){ style.nm = nm; _remergeStyle(); }
	
	UIC_DEFINE_ACCESSORS( float, x );
	UIC_DEFINE_ACCESSORS( float, y );
	UIC_DEFINE_ACCESSORS( float, width );
	UIC_DEFINE_ACCESSORS( float, height );
	UIC_DEFINE_ACCESSORS( float, q0x );
	UIC_DEFINE_ACCESSORS( float, q0y );
	UIC_DEFINE_ACCESSORS( float, q1x );
	UIC_DEFINE_ACCESSORS( float, q1y );
	UIC_DEFINE_ACCESSORS( float, nc_top );
	UIC_DEFINE_ACCESSORS( float, nc_left );
	UIC_DEFINE_ACCESSORS( float, nc_right );
	UIC_DEFINE_ACCESSORS( float, nc_bottom );
	UIC_DEFINE_ACCESSORS( bool, visible );
	UIC_DEFINE_ACCESSORS( int, index );
	UIC_DEFINE_ACCESSORS( bool, topmost );
	UIC_DEFINE_ACCESSORS( float, minWidth );
	UIC_DEFINE_ACCESSORS( float, maxWidth );
	UIC_DEFINE_ACCESSORS( float, minHeight );
	UIC_DEFINE_ACCESSORS( float, maxHeight );
	UIC_DEFINE_ACCESSORS( float, marginLeft );
	UIC_DEFINE_ACCESSORS( float, marginRight );
	UIC_DEFINE_ACCESSORS( float, marginTop );
	UIC_DEFINE_ACCESSORS( float, marginBottom );
	UIC_DEFINE_ACCESSORS( float, paddingLeft );
	UIC_DEFINE_ACCESSORS( float, paddingRight );
	UIC_DEFINE_ACCESSORS( float, paddingTop );
	UIC_DEFINE_ACCESSORS( float, paddingBottom );
	UIC_DEFINE_ACCESSORS2( sgsVariable, cursor );
	UIC_DEFINE_ACCESSORS2( sgsString, font );
	UIC_DEFINE_ACCESSORS( float, fontSize );
	UIC_DEFINE_ACCESSORS2( sgsVariable, renderfunc );
	
	float get_offsetLeft(){ return get_x(); }
	float get_offsetRight(){ return -get_width() - get_x(); }
	float get_offsetTop(){ return get_y(); }
	float get_offsetBottom(){ return -get_height() - get_y(); }
	void set_offsetLeft( float v ){ style.width.set( get_width() - v + get_x() ); set_x( v ); }
	void set_offsetRight( float v ){ set_width( -v - get_x() ); }
	void set_offsetTop( float v ){ style.height.set( get_height() - v + get_y() ); set_y( v ); }
	void set_offsetBottom( float v ){ set_height( -v - get_y() ); }
	
	SGS_PROPERTY_FUNC( READ get_x WRITE set_x ) SGS_ALIAS( float x );
	SGS_PROPERTY_FUNC( READ get_y WRITE set_y ) SGS_ALIAS( float y );
	SGS_PROPERTY_FUNC( READ get_width WRITE set_width ) SGS_ALIAS( float width );
	SGS_PROPERTY_FUNC( READ get_height WRITE set_height ) SGS_ALIAS( float height );
	SGS_PROPERTY_FUNC( READ get_q0x WRITE set_q0x ) SGS_ALIAS( float q0x );
	SGS_PROPERTY_FUNC( READ get_q0y WRITE set_q0y ) SGS_ALIAS( float q0y );
	SGS_PROPERTY_FUNC( READ get_q1x WRITE set_q1x ) SGS_ALIAS( float q1x );
	SGS_PROPERTY_FUNC( READ get_q1y WRITE set_q1y ) SGS_ALIAS( float q1y );
	SGS_PROPERTY_FUNC( READ get_nc_top WRITE set_nc_top ) SGS_ALIAS( float nc_top );
	SGS_PROPERTY_FUNC( READ get_nc_left WRITE set_nc_left ) SGS_ALIAS( float nc_left );
	SGS_PROPERTY_FUNC( READ get_nc_right WRITE set_nc_right ) SGS_ALIAS( float nc_right );
	SGS_PROPERTY_FUNC( READ get_nc_bottom WRITE set_nc_bottom ) SGS_ALIAS( float nc_bottom );
	SGS_PROPERTY_FUNC( READ get_visible WRITE set_visible ) SGS_ALIAS( bool visible );
	SGS_PROPERTY_FUNC( READ get_index WRITE set_index ) SGS_ALIAS( int index );
	SGS_PROPERTY_FUNC( READ get_topmost WRITE set_topmost ) SGS_ALIAS( bool topmost );
	SGS_PROPERTY_FUNC( READ get_minWidth WRITE set_minWidth ) SGS_ALIAS( float minWidth );
	SGS_PROPERTY_FUNC( READ get_maxWidth WRITE set_maxWidth ) SGS_ALIAS( float maxWidth );
	SGS_PROPERTY_FUNC( READ get_minHeight WRITE set_minHeight ) SGS_ALIAS( float minHeight );
	SGS_PROPERTY_FUNC( READ get_maxHeight WRITE set_maxHeight ) SGS_ALIAS( float maxHeight );
	SGS_PROPERTY_FUNC( READ get_marginLeft WRITE set_marginLeft ) SGS_ALIAS( float marginLeft );
	SGS_PROPERTY_FUNC( READ get_marginRight WRITE set_marginRight ) SGS_ALIAS( float marginRight );
	SGS_PROPERTY_FUNC( READ get_marginTop WRITE set_marginTop ) SGS_ALIAS( float marginTop );
	SGS_PROPERTY_FUNC( READ get_marginBottom WRITE set_marginBottom ) SGS_ALIAS( float marginBottom );
	SGS_PROPERTY_FUNC( READ get_paddingLeft WRITE set_paddingLeft ) SGS_ALIAS( float paddingLeft );
	SGS_PROPERTY_FUNC( READ get_paddingRight WRITE set_paddingRight ) SGS_ALIAS( float paddingRight );
	SGS_PROPERTY_FUNC( READ get_paddingTop WRITE set_paddingTop ) SGS_ALIAS( float paddingTop );
	SGS_PROPERTY_FUNC( READ get_paddingBottom WRITE set_paddingBottom ) SGS_ALIAS( float paddingBottom );
	SGS_PROPERTY_FUNC( READ get_cursor WRITE set_cursor ) SGS_ALIAS( sgsVariable cursor );
	SGS_PROPERTY_FUNC( READ get_font WRITE set_font ) SGS_ALIAS( sgsString font );
	SGS_PROPERTY_FUNC( READ get_fontSize WRITE set_fontSize ) SGS_ALIAS( float fontSize );
	SGS_PROPERTY_FUNC( READ get_renderfunc WRITE set_renderfunc ) SGS_ALIAS( sgsVariable renderfunc );
	
	SGS_PROPERTY_FUNC( READ WRITE WRITE_CALLBACK updateLayout ) float scroll_x;
	SGS_PROPERTY_FUNC( READ WRITE WRITE_CALLBACK updateLayout ) float scroll_y;
	SGS_PROPERTY_FUNC( READ WRITE WRITE_CALLBACK updateLayout ) bool nonclient;
	SGS_PROPERTY_FUNC( READ get_offsetLeft WRITE set_offsetLeft ) SGS_ALIAS( float offsetLeft );
	SGS_PROPERTY_FUNC( READ get_offsetRight WRITE set_offsetRight ) SGS_ALIAS( float offsetRight );
	SGS_PROPERTY_FUNC( READ get_offsetTop WRITE set_offsetTop ) SGS_ALIAS( float offsetTop );
	SGS_PROPERTY_FUNC( READ get_offsetBottom WRITE set_offsetBottom ) SGS_ALIAS( float offsetBottom );
	
	SGS_PROPERTY sgsVariable _cachedFont;
	SGS_METHOD void setFont( const sgsString& newFont, float newFontSize ){ style.font = newFont; set_fontSize( newFontSize ); }
	
	SGS_PROPERTY sgsVariable callback;
	SGS_PROPERTY sgsVariable data;
	SGS_PROPERTY sgsVariable _interface;
	
	SGS_PROPERTY READ float rx0;
	SGS_PROPERTY READ float rx1;
	SGS_PROPERTY READ float ry0;
	SGS_PROPERTY READ float ry1;
	
	SGS_PROPERTY bool _updatingLayout : 1; /* true if updating layout and don't want to trigger further layout changes */
	SGS_PROPERTY bool _roundedCoords : 1; /* true if final coords (r[xy][01]) should be rounded */
	SGS_PROPERTY READ bool mouseOn;
	SGS_PROPERTY READ bool clicked;
	SGS_PROPERTY READ bool keyboardFocus;
	
	HandleArray m_children;
	HandleArray m_sorted;
	sgsVariable m_events;
	AnimArray m_animQueue;
};



inline void UI_PushEvent( SGS_CTX, UIEvent* e ){ sgs_PushLiteClassFrom( C, e ); }


#endif // __SGS_UI_CONTROL__

