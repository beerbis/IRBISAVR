unit uMain;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants, System.Classes, Vcl.Graphics,
  Vcl.Controls, Vcl.Forms, Vcl.Dialogs, Vcl.StdCtrls;

type
  TForm1 = class(TForm)
    Edit1: TEdit;
    Edit2: TEdit;
    DoIt: TButton;
    cbAzbura: TComboBox;
    cbOutFormat: TComboBox;
    procedure DoItClick(Sender: TObject);
    procedure Edit2Enter(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}
Type
  TFormatByteProc = function (InChar: Cardinal): AnsiString;
  function DollarHex(InChar: Cardinal): AnsiString;
  begin
    Result := '$'+IntToHex(InChar, 2)+', ';
  end;

  function NormalizeChar(InChar, FirstChar: AnsiChar): Cardinal;
  begin
    if InChar in ['0'..'9'] then InChar := ' ';
    if InChar = ' ' then Result := 0
    else begin
      Result := Ord(InChar) - Ord(FirstChar)+1;
    end;
  end;


procedure TForm1.DoItClick(Sender: TObject);
var
  source,
  destination: AnsiString;
  i: Integer;
  firstChar: AnsiChar;
  OutFormat: TFormatByteProc;

const
  enFirst: AnsiChar = 'A';
  ruFirst: AnsiChar = 'А';
begin
  case cbAzbura.ItemIndex of
    0: firstChar := enFirst;
    1: firstChar := ruFirst;
  end;
  case cbOutFormat.ItemIndex of
  2: OutFormat := @DollarHex;
  else Assert(False, 'Пардон, не готово.');
  end;
  source := Trim(Edit1.Text);
  source := AnsiUpperCase(source);
  destination := '';
  for i := 1 to Length(source) do
    destination := destination + OutFormat(
      NormalizeChar(source[i], firstChar)
    );
  Edit2.Text := destination;
end;

procedure TForm1.Edit2Enter(Sender: TObject);
begin
  Edit2.SelStart:= 0;
  Edit2.SelLength := Length(Edit2.Text)
end;

end.
