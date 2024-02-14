object gui: Tgui
  Left = 0
  Top = 0
  Caption = 'Art Gallery Problem'
  ClientHeight = 687
  ClientWidth = 1000
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  TextHeight = 13
  object image: TImage
    Left = 0
    Top = 0
    Width = 800
    Height = 633
    OnMouseDown = image_mouse_down
    OnMouseUp = image_mouse_up
  end
  object Label1: TLabel
    Left = 40
    Top = 649
    Width = 63
    Height = 16
    Caption = 'Coverage: '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label2: TLabel
    Left = 277
    Top = 649
    Width = 124
    Height = 16
    Caption = 'Number of Cameras: '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label3: TLabel
    Left = 832
    Top = 461
    Width = 69
    Height = 13
    Caption = 'Camera radius'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label4: TLabel
    Left = 832
    Top = 490
    Width = 79
    Height = 13
    Caption = 'Camera fill angle'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label5: TLabel
    Left = 832
    Top = 515
    Width = 83
    Height = 13
    Caption = 'Camera accuracy'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object drawing_polygon: TRadioButton
    Left = 824
    Top = 8
    Width = 153
    Height = 25
    Caption = 'Draw Polygon'
    TabOrder = 0
  end
  object drawing_hole: TRadioButton
    Left = 824
    Top = 39
    Width = 153
    Height = 25
    Caption = 'Draw hole'
    TabOrder = 1
  end
  object drawing_camera: TRadioButton
    Left = 824
    Top = 70
    Width = 153
    Height = 25
    Caption = 'Draw Camera'
    TabOrder = 2
  end
  object finish_object: TButton
    Left = 824
    Top = 112
    Width = 153
    Height = 33
    Caption = 'Finish Object'
    TabOrder = 3
    OnClick = finish_object_click
  end
  object draw_simple_campus: TButton
    Left = 824
    Top = 151
    Width = 153
    Height = 33
    Caption = 'Draw Simple Campus'
    TabOrder = 4
    OnClick = draw_simple_campus_click
  end
  object draw_complex_campus: TButton
    Left = 824
    Top = 190
    Width = 153
    Height = 33
    Caption = 'Draw Complex Campus'
    TabOrder = 5
    OnClick = draw_complex_campus_click
  end
  object color_vertices_agp: TButton
    Left = 824
    Top = 229
    Width = 153
    Height = 33
    Caption = 'Color Vertices AGP'
    TabOrder = 6
    OnClick = color_vertices_agp_click
  end
  object area_based_agp: TButton
    Left = 824
    Top = 268
    Width = 153
    Height = 33
    Caption = 'Area-based AGP'
    TabOrder = 7
    OnClick = area_based_agp_click
  end
  object advance_area_based_agp: TButton
    Left = 824
    Top = 307
    Width = 153
    Height = 33
    Caption = 'Advance Area-based AGP'
    TabOrder = 8
    OnClick = advance_area_based_agp_click
  end
  object check_position_agp: TButton
    Left = 824
    Top = 346
    Width = 153
    Height = 33
    Caption = 'Check Position AGP'
    TabOrder = 9
    OnClick = check_position_agp_click
  end
  object clean_screen: TButton
    Left = 824
    Top = 635
    Width = 153
    Height = 33
    Caption = 'Clean Screen'
    TabOrder = 10
    OnClick = clean_screen_click
  end
  object draw_delaunay_triangulation: TButton
    Left = 824
    Top = 596
    Width = 153
    Height = 33
    Caption = 'Draw Delaunay Triangulation'
    TabOrder = 11
    OnClick = draw_delaunay_triangulation_click
  end
  object coverage_percentage: TEdit
    Left = 109
    Top = 648
    Width = 44
    Height = 21
    TabOrder = 12
  end
  object num_cameras: TEdit
    Left = 407
    Top = 648
    Width = 44
    Height = 21
    TabOrder = 13
  end
  object camera_radius: TEdit
    Left = 917
    Top = 458
    Width = 44
    Height = 21
    TabOrder = 14
  end
  object camera_fill_angle: TEdit
    Left = 917
    Top = 485
    Width = 44
    Height = 21
    TabOrder = 15
  end
  object camera_accuracy: TEdit
    Left = 917
    Top = 512
    Width = 44
    Height = 21
    TabOrder = 16
  end
  object calculate_coverage: TButton
    Left = 824
    Top = 557
    Width = 153
    Height = 33
    Caption = 'Calculate Coverage'
    TabOrder = 17
    OnClick = calculate_coverage_click
  end
  object genetic_agp: TButton
    Left = 824
    Top = 385
    Width = 153
    Height = 33
    Caption = 'Genetic AGP'
    TabOrder = 18
    OnClick = genetic_agp_click
  end
  object inputNumCameras: TEdit
    Left = 824
    Top = 434
    Width = 153
    Height = 21
    TabOrder = 19
    TextHint = 'Input number of cameras'
  end
end
