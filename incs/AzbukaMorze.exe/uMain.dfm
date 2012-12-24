object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'Form1'
  ClientHeight = 202
  ClientWidth = 445
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Edit1: TEdit
    Left = 24
    Top = 8
    Width = 401
    Height = 21
    TabOrder = 0
    Text = #1057' '#1085#1086#1074#1099#1084' '#1075#1086#1076#1086#1084
  end
  object Edit2: TEdit
    Left = 24
    Top = 35
    Width = 401
    Height = 21
    Color = clInfoBk
    ReadOnly = True
    TabOrder = 1
    Text = 'Edit2'
    OnEnter = Edit2Enter
  end
  object DoIt: TButton
    Left = 350
    Top = 74
    Width = 75
    Height = 25
    Caption = 'DoIt'
    TabOrder = 2
    OnClick = DoItClick
  end
  object cbAzbura: TComboBox
    Left = 24
    Top = 62
    Width = 145
    Height = 21
    Style = csDropDownList
    ItemIndex = 1
    TabOrder = 3
    Text = 'AzbukaRu'
    Items.Strings = (
      'AzbukaEn'
      'AzbukaRu')
  end
  object cbOutFormat: TComboBox
    Left = 24
    Top = 89
    Width = 145
    Height = 21
    Style = csDropDownList
    ItemIndex = 2
    TabOrder = 4
    Text = #1041#1072#1082#1089'-'#1082#1077#1082#1089#1099' '#1089' '#1079#1072#1087#1103#1090#1086#1081
    Items.Strings = (
      #1057#1090#1088#1086#1082#1086#1081
      #1050#1077#1082#1089#1099
      #1041#1072#1082#1089'-'#1082#1077#1082#1089#1099' '#1089' '#1079#1072#1087#1103#1090#1086#1081)
  end
end
