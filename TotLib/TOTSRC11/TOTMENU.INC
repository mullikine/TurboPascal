{               Copyright 1991 TechnoJock Software, Inc.               }
{                          All Rights Reserved                         }
{                         Restricted by License                        }

{File TOTMENU.INC}

{|||||||||||||||||||||||||||||||||||||||||||||||}
{                                               }
{     B a s e M e n u O B J   M E T H O D S     }
{                                               }
{|||||||||||||||||||||||||||||||||||||||||||||||}
constructor BaseMenuOBJ.Init;
{}
begin
   vItemStack := nil; 
   vTotalItems := 0;
   vActiveItem := 0;
   vAllowEsc := true;
   vUsedInPull := false;
   vMsgVisible := false;
   vSubActive := false;
   vMenuHiHot := LookTOT^.MenuHiHot;
   vMenuHiNorm := LookTOT^.MenuHiNorm;
   vMenuLoHot := LookTOT^.MenuLoHot;
   vMenuLoNorm := LookTOT^.MenuLoNorm;
   vMenuOff := LookTOT^.MenuOff;
   vPickOff := true;
   vGap := 2;
   vWidth := 0;
   vX := 0;
   vY := 0;
   vMsgX := 0;
   vMsgY := Monitor^.Depth;
   vHelpKey := 315;
   vhelpHook := NoHelpHook;
end; {BaseMenuOBJ.Init}

function BaseMenuOBJ.ItemPtr(Item:byte): MenuItemPtr;
{}
var 
   Temp: MenuItemPtr;
   I: integer;
begin
   if (Item < 1) or (Item > vTotalItems) then
      ItemPtr := nil
   else
   begin
      Temp := vItemStack;
      if Item > 1 then
         for I := 2 to Item do
             if Temp <> nil then
                Temp := Temp^.NextNode;
      ItemPtr := Temp;
   end;
end; {BaseMenuOBJ.ItemPtr}

function BaseMenuOBJ.FirstActiveItem: byte;
{}
var 
   Temp: MenuItemPtr;
   I: integer;
begin
   Temp := vItemStack;
   if Temp = nil then
      FirstActiveItem := 0
   else
   begin
      I := 1;
      while (Temp <> nil) and (Temp^.Active = false) do
      begin
         inc(I);
         Temp := Temp^.NextNode;
      end;
      FirstActiveItem := I;
   end;
end; {BaseMenuOBJ.FirstActiveItem}

procedure BaseMenuOBJ.AddFullItem(Txt:StrVisible; ID,HK:word; Msg:StrVisible; SubM:BaseMenuPtr);
{}
begin
   AddItem(Txt);
   SetID(vTotalItems,ID);
   SetHK(vTotalItems,HK);
   SetMessage(vTotalItems,Msg);
   SetSubMenu(vTotalItems,SubM);
end; {BaseMenuOBJ.AddFullItem}

procedure BaseMenuOBJ.AddItem(Txt:StrVisible);
{}
var 
   Temp: MenuItemPtr;
   L : byte;
begin
   if MaxAvail < sizeof(vItemStack^)+succ(length(Txt)) then
      exit
   else
   begin
      if vItemStack = nil then
      begin
         getmem(vItemStack,sizeof(vItemStack^));
         vActiveItem := 1;
         Temp := vItemStack;
      end
      else
      begin
         Temp := ItemPtr(vTotalItems);
         getmem(Temp^.NextNode, sizeof(Temp^));
         Temp := Temp^.NextNode;
      end;
      Temp^.NextNode := nil;
      inc(vTotalItems);
      with Temp^ do
      begin
         L := succ(length(Txt));
         if L = 1 then
           TxtPtr := nil
         else
         begin
            if MemAvail >=  L then
            begin
               getmem(TxtPtr,L);
               move(Txt[0],TxtPtr^,L);
            end;
         end;  
         MsgPtr := nil;
         HK :=  0;
         ID := 0;
         if (Txt = '-') or (Txt = '=') or (Txt = '') then
            Active := false
         else
            Active := true;
         SubMenu := nil;
      end;
      L := length(strip('A',Screen.HiMarker,Txt));
      if L > vWidth then
         vWidth := L;
   end;
end; {BaseMenuOBJ.AddItem}

procedure BaseMenuOBJ.SetTopic(Item:byte; Txt:StrVisible);
{}
var 
   Temp: MenuItemPtr;
   L: byte;
begin
   Temp := ItemPtr(Item);
   if Temp <> nil then
   begin
      if Temp^.TxtPtr <> nil then
      begin
         move(Temp^.TxtPtr^,L,1);
         freemem(Temp^.TxtPtr,L);
      end;
      L := succ(length(Txt));
      if memavail >= L then
      begin
         if L = 1 then
           Temp^.TxtPtr := nil
         else
         begin
            getmem(Temp^.TxtPtr,L);
            move(Txt[0],Temp^.TxtPtr^,L);
         end;
      end;
   end;
end; {BaseMenuOBJ.SetTopic}

procedure BaseMenuOBJ.SetMessage(Item:byte; Msg:StrVisible);
{}
var 
   Temp: MenuItemPtr;
   L: byte;
begin
   Temp := ItemPtr(Item);
   if Temp <> nil then
   begin
      if Temp^.MsgPtr <> nil then
      begin
         move(Temp^.MsgPtr^,L,1);
         freemem(Temp^.MsgPtr,L);
      end;
      L := succ(length(Msg));
      if memavail >= L then
      begin
         if L = 1 then
           Temp^.MsgPtr := nil
         else
         begin
            getmem(Temp^.MsgPtr,L);
            move(Msg[0],Temp^.MsgPtr^,L);
         end;
      end;
   end;
end; {BaseMenuOBJ.SetMessage}

procedure BaseMenuOBJ.SetHK(Item:byte; HK:word);
{}
var Temp: MenuItemPtr;
begin
   Temp := ItemPtr(Item);
   if Temp <> nil then
      Temp^.HK := HK;
end; {BaseMenuOBJ.SetHK}

procedure BaseMenuOBJ.SetID(Item:byte; ID:word);
{}
var Temp: MenuItemPtr;
begin
   Temp := ItemPtr(Item);
   if Temp <> nil then
      Temp^.ID := ID;
end; {BaseMenuOBJ.SetID}

procedure BaseMenuOBJ.SetStatus(Item:byte; On:boolean);
{}
var Temp: MenuItemPtr;
begin
   Temp := ItemPtr(Item);
   if Temp <> nil then
      Temp^.Active := On;
end; {BaseMenuOBJ.SetStatus}

procedure BaseMenuOBJ.SetSubMenu(Item:byte;SubMenu:BaseMenuPtr);
{}
var 
  Temp: MenuItemPtr;
  L: byte;
  Str: StrVisible;
begin
   Temp := ItemPtr(Item);
   if Temp <> nil then
   begin
      Temp^.SubMenu := SubMenu;
      Str := GetText(Temp);
      L := succ(length(strip('A',Screen.HiMarker,Str)));
      if L > vWidth then
         vWidth := L;
   end;
end; {BaseMenuOBJ.SetSubMenu}

procedure BaseMenuOBJ.SetGap(G:byte);
{}
begin
   vGap := G;
end; {BaseMenuOBJ.SetGap}

procedure BaseMenuOBJ.SetActiveItem(Item:byte);
{}
begin
   if Item in [1..vTotalItems] then
      vActiveItem := Item;
end; {BaseMenuOBJ.SetActiveItem}

procedure BaseMenuOBJ.SetMessageXY(X,Y:byte);
{}
begin
   vMsgX := X;
   vMsgY := Y;
end; {BaseMenuOBJ.SetMessageXY}

procedure BaseMenuOBJ.SetMenuXY(X,Y:byte);
{}
begin
   vX := X;
   vY := Y;
end; {BaseMenuOBJ.SetMenuXY}

procedure BaseMenuOBJ.SetHelpKey(K:word);
{}
begin
   vHelpKey := K;
end; {BaseMenuOBJ.SetHelpKey}

procedure BaseMenuOBJ.SetAllowEsc(On:boolean);
{}
begin
   vAllowEsc := On;
end; {BaseMenuOBJ.SetAllowEsc}

procedure BaseMenuOBJ.SetColors(HiHot,HiNorm,LoHot,LoNorm,Off:byte);
{}
begin
   vMenuHiHot := HiHot;
   vMenuHiNorm := HiNorm;
   vMenuLoHot := LoHot;
   vMenuLoNorm := LoNorm;
   vMenuOff := Off;
end; {BaseMenuOBJ.SetColors}

function BaseMenuOBJ.GetAllowEsc: boolean;
{}
begin
   GetAllowEsc := vAllowEsc;
end; {BaseMenuOBJ.GetAllowEsc}

function BaseMenuOBJ.GetText(Ptr:MenuItemPtr): StrVisible;
{}
var 
  Str: StrVisible;
  L : byte;
begin
   Str := '';
   if Ptr <> nil then
      if Ptr^.TxtPtr <> nil then
      begin
         move(Ptr^.TxtPtr^,L,1);
         if L > 0 then
            move(Ptr^.TxtPtr^,Str,succ(L));
      end;
   GetText := Str;
end; {BaseMenuOBJ.GetText}

function BaseMenuOBJ.GetMessage(Ptr:MenuItemPtr): StrVisible;
{}
var 
  Str: StrVisible;
  L : byte;
begin
   Str := '';
   if Ptr <> nil then
      if Ptr^.MsgPtr <> nil then
      begin
         move(Ptr^.MsgPtr^,L,1);
         if L > 0 then
            move(Ptr^.MsgPtr^,Str,succ(L));
      end;
   GetMessage := Str;
end; {BaseMenuOBJ.GetMessage}

function BaseMenuOBJ.GetID(Item:byte):word;
{}
var Temp: MenuItemPtr;
begin
   Temp := ItemPtr(Item);
   if Temp <> nil then
      GetID := Temp^.ID
   else
      GetID := 0;
end; {BaseMenuOBJ.GetID}

function BaseMenuOBJ.GetHelpID:word;
{}
var 
  Temp: MenuItemPtr;
   Sub: BaseMenuPtr;
begin
   Temp := ItemPtr(vActiveItem);
   if Temp <> nil then
   begin
      Sub := Temp^.Submenu;
      if (Sub <> nil) and vSubActive then
         GetHelpID := Sub^.GetHelpID
      else
         GetHelpID := Temp^.ID;
   end
   else
      GetHelpID := 0;
end; {BaseMenuOBJ.GetHelpID}

function BaseMenuOBJ.GetActiveItem: byte;
{}
begin
   GetActiveItem := vActiveItem;
end; {BaseMenuOBJ.GetActiveItem}

function BaseMenuOBJ.GetTotalItems: byte;
{}
begin
   GetTotalItems := vTotalItems;
end; {BaseMenuOBJ.GetTotalItems}

procedure BaseMenuOBJ.DisplayAllItems;
{}
var
  I: integer;
  L,C,R: boolean;
  X,Y:byte;
begin
   if vUsedInPull then
   begin
      for I := 1 to vTotalItems do
          DisplayItem(I,false,false);
      Mouse.Status(L,C,R,X,Y);
      if L then
         vPickOff := true
      else
      begin
         DisplayItem(vActiveItem,true,true);
         vPickOff := false;
      end;
   end
   else
      for I := 1 to vTotalItems do
          DisplayItem(I,vActiveItem=I,vActiveItem=I);
end; {BaseMenuOBJ.DisplayAllItems}

procedure BaseMenuOBJ.TurnPickOff;
{}
var
   Sub: BaseMenuPtr;
begin
   if vUsedInPull and vSubActive then
   begin
      Sub := ItemPtr(vActiveItem)^.SubMenu;
      if Sub <> nil then
         Sub^.Remove;
      vSubActive := false
   end;
   vPickOff := true;
   DisplayItem(vActiveItem,false,true);
end; {BaseMenuOBJ.TurnPickOff}

function BaseMenuOBJ.GetPickOff: boolean;
{}
begin
   GetPickOff := vPickOff;
end; {BaseMenuOBJ.GetPickOff}

function BaseMenuOBJ.GetSubActive: boolean;
{}
begin
   GetSubActive := vSubActive;
end; {BaseMenuOBJ.GetSubActive}

procedure BaseMenuOBJ.ChangeActiveItem(New:byte);
{}
begin
   if (New <> vActiveItem) or vPickOff then
   begin
      DisplayItem(vActiveItem,false,true);
      vActiveItem := New;
      DisplayItem(vActiveItem,true,true);
      vPickOff := false;
   end;
end; {BaseMenuOBJ.ChangeActiveItem}

function BaseMenuOBJ.HotkeySelect(K:word): boolean;
{}
var
   I : byte;
   Temp: MenuItemPtr;
begin
   Temp := vItemStack;
   I := 1;
   if (K > 96) and (K <123) then
      K := K - 32;
   while Temp <> Nil do
   begin
      if (Temp^.HK = K) and Temp^.Active then
      begin
         if I <> vActiveItem then
            ChangeActiveItem(I);
         HotKeySelect := true;
         exit;
      end
      else
      begin
         Temp := Temp^.NextNode;
         inc(I);
      end;   
   end;
   HotKeySelect := false;
end; {BaseMenuOBJ.HotkeySelect}

function BaseMenuOBJ.AddPre(Txt:StrVisible;Hi:boolean):StrVisible;
{}
var Pre : Char;
begin
   Pre := LookTOT^.ListLeftChar;
   if Pre <> #0 then
   begin
      if Hi then 
         Txt := Pre+Txt
      else
         Txt := ' '+Txt;
   end;
   AddPre := Txt;
end; {BaseMenuOBJ.AddPre}

function BaseMenuOBJ.AddSuf(Txt:StrVisible;Hi:boolean):StrVisible;
{}
var Suf : Char;
begin
   Suf := LookTOT^.ListRightChar;
   if Suf <> #0 then
   begin
      if Hi then 
         Txt := Txt+Suf
      else
         Txt := Txt+' ';
   end;
   AddSuf := Txt;
end; {BaseMenuOBJ.AddSuf}

procedure BaseMenuOBJ.DisposeItems;
{}
var 
   Tmp1,Tmp2: MenuItemPtr;
   L : byte;
begin
   Tmp1 := vItemStack;
   while Tmp1 <> nil do
   begin
      Tmp2 := Tmp1^.NextNode;
      if Tmp1^.TxtPtr <> Nil then
      begin
         move(Tmp1^.TxtPtr^,L,1);
         freemem(Tmp1^.TxtPtr,succ(L));
      end;
      if Tmp1^.MsgPtr <> Nil then
      begin
         move(Tmp1^.MsgPtr^,L,1);
         freemem(Tmp1^.MsgPtr,succ(L));
      end;
      freemem(Tmp1,sizeof(Tmp1^));
      Tmp1 := Tmp2;
   end;
   vItemStack := nil;
   vTotalItems := 0;
   vActiveItem := 0;
end; {BaseMenuOBJ.DisposeItems}

procedure BaseMenuOBJ.ChangeMessage(Item:byte; Hi:boolean);
{}
var
   Temp: MenuItemPtr;
   Txt: StrVisible;
   WinWasActive: boolean;
   Col: byte;
begin
   Temp := ItemPtr(Item);
   Txt := GetMessage(Temp);          
   Col := IOTOT^.MessageCol;
   if Txt <> '' then
   begin
      WinWasActive := Screen.WindowOff;
      if Hi then
      begin
         if vMsgX = 0 then
            Screen.WriteCenter(vMsgY,Col,Txt)
         else
            Screen.WriteAT(vMsgX,vMsgY,Col,Txt);
         vMsgVisible := true;
      end
      else
      begin
         if vMsgX = 0 then
            Screen.WriteCenter(vMsgY,Col,replicate(length(Txt),' '))
         else
            Screen.WriteAT(vMsgX,vMsgY,Col,replicate(length(Txt),' '));
         vMsgVisible := false;
      end;
      Screen.WindowOn;
   end;
end; {BaseMenuOBJ.ChangeMessage}

function BaseMenuOBJ.LastKey: word;
{}
begin
   LastKey := vLastKey;
end; {BaseMenuOBJ.LastKey}

procedure BaseMenuOBJ.SetHelpHook(Proc:HelpProc);
{}
begin
   vHelpHook := Proc;
end; {BaseMenuOBJ.SetHelpHook}

procedure BaseMenuOBJ.HelpTask(ID:word);
{}
begin
   vHelpHook(ID);
end; {BaseMenuOBJ.HelpTask}

procedure BaseMenuOBJ.SetForPull;
{abstract} begin end; 

procedure BaseMenuOBJ.DisplayItem(Item:byte;Hi,Msg:boolean);                
{abstract} begin end; 

procedure BaseMenuOBJ.Remove;                
{abstract} begin end; 

function BaseMenuOBJ.Activate: word;        
{abstract} begin end; 

procedure BaseMenuOBJ.DrawEngine(eX,eY:byte);
{abstract} begin end; 

function BaseMenuOBJ.TargetPick(X,Y:byte):byte;
{abstract} begin end; 

function BaseMenuOBJ.ProcessKey(K:word; X,Y:byte):word;
{abstract} begin end; 

function BaseMenuOBJ.MenuZone(X,Y:byte):boolean;
{abstract} 
begin 
   MenuZone := false;
end; {BaseMenuOBJ.MenuZone}

destructor BaseMenuOBJ.Done;
{}
begin
   DisposeItems;
end; {BaseMenuOBJ.Done}