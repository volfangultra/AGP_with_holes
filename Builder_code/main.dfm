object agp_aplication: Tagp_aplication
  Left = 0
  Top = 0
  Caption = 'AGP'
  ClientHeight = 685
  ClientWidth = 968
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = True
  PixelsPerInch = 96
  TextHeight = 13
  object image: TImage
    Left = 8
    Top = 8
    Width = 753
    Height = 601
    OnMouseDown = image_mouse_down
    OnMouseMove = image_mouse_move
    OnMouseUp = image_mouse_up
  end
  object label_coordinates: TLabel
    Left = 707
    Top = 632
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
    Left = 16
    Top = 633
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
  object label_num_cameras: TLabel
    Left = 263
    Top = 633
    Width = 115
    Height = 16
    Caption = 'Number of Cameras'
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
    Top = 488
    Width = 164
    Height = 25
    Caption = 'Clear'
    TabOrder = 6
    OnClick = button_clean_click
  end
  object text_coordinate: TEdit
    Left = 781
    Top = 631
    Width = 163
    Height = 21
    TabOrder = 7
  end
  object text_surface_area_covered: TEdit
    Left = 145
    Top = 632
    Width = 80
    Height = 21
    TabOrder = 8
  end
  object ButtonPolygonArea: TButton
    Left = 775
    Top = 182
    Width = 163
    Height = 25
    Caption = 'Polygon Area'
    TabOrder = 9
    OnClick = ButtonPolygonAreaClick
  end
  object Button1: TButton
    Left = 775
    Top = 213
    Width = 163
    Height = 25
    Caption = 'Cameras intersection'
    TabOrder = 10
    OnClick = CamerasIntersectionClick
  end
  object Button2: TButton
    Left = 775
    Top = 244
    Width = 163
    Height = 25
    Caption = 'Draw Campus'
    TabOrder = 11
    OnClick = ButtonDrawCampusClick
  end
  object text_num_cameras: TEdit
    Left = 384
    Top = 631
    Width = 41
    Height = 21
    TabOrder = 12
  end
  object ButtonDelaunayTriangulation: TButton
    Left = 776
    Top = 275
    Width = 163
    Height = 25
    Caption = 'Delone trianguliraj'
    TabOrder = 13
    OnClick = ButtonDelaunayTriangulationClick
  end
  object coverArtGallery: TButton
    Left = 776
    Top = 337
    Width = 163
    Height = 25
    Caption = 'Cover Art Gallery'
    TabOrder = 14
    OnClick = coverArtGalleryClick
  end
  object button_color_vertices: TButton
    Left = 776
    Top = 306
    Width = 163
    Height = 25
    Caption = 'Color vertices'
    TabOrder = 15
    OnClick = button_color_verticesClick
  end
end
