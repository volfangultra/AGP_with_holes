object agp_aplication: Tagp_aplication
  Left = 0
  Top = 0
  Caption = 'AGP'
  ClientHeight = 635
  ClientWidth = 952
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  TextHeight = 13
  object image: TImage
    Left = 8
    Top = 8
    Width = 753
    Height = 505
    OnMouseDown = image_mouse_down
    OnMouseMove = image_mouse_move
    OnMouseUp = image_mouse_up
  end
  object label_coordinates: TLabel
    Left = 701
    Top = 561
    Width = 68
    Height = 16
    Caption = 'Coordinates'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object label_surface_area_covered: TLabel
    Left = 8
    Top = 527
    Width = 123
    Height = 16
    Caption = 'Surface area covered'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object input_num_vertices_polygon: TEdit
    Left = 775
    Top = 24
    Width = 83
    Height = 21
    TabOrder = 0
  end
  object generate: TButton
    Left = 864
    Top = 24
    Width = 75
    Height = 21
    Caption = 'Generate'
    TabOrder = 1
  end
  object radio_draw_hole: TRadioButton
    Left = 775
    Top = 96
    Width = 113
    Height = 17
    Caption = 'Draw hole'
    TabOrder = 2
  end
  object radio_draw_polygon: TRadioButton
    Left = 775
    Top = 64
    Width = 113
    Height = 17
    Caption = 'Draw Polygon'
    TabOrder = 3
  end
  object radio_add_camera: TRadioButton
    Left = 775
    Top = 128
    Width = 113
    Height = 17
    Caption = 'Add Camera'
    TabOrder = 4
  end
  object button_finish_object: TButton
    Left = 775
    Top = 151
    Width = 164
    Height = 25
    Caption = 'Finish Object'
    TabOrder = 5
    OnClick = button_finish_object_click
  end
  object button_clean: TButton
    Left = 775
    Top = 496
    Width = 164
    Height = 25
    Caption = 'Clear'
    TabOrder = 6
    OnClick = button_clean_click
  end
  object text_coordinate: TEdit
    Left = 775
    Top = 552
    Width = 163
    Height = 21
    TabOrder = 7
  end
  object text_surface_area_covered: TEdit
    Left = 137
    Top = 518
    Width = 80
    Height = 21
    TabOrder = 8
  end
  object Button1: TButton
    Left = 792
    Top = 192
    Width = 113
    Height = 25
    Caption = 'Polygons intersection'
    TabOrder = 9
    OnClick = PolygonsIntersectionClick
  end
end
