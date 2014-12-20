//
// Downloaded from http://www.doogal.co.uk/LcdLabel.php
//

using System;
using System.Drawing;
using System.Windows.Forms;

namespace LCDLabel
{
  /// <summary>
  /// Enumerated type defining the different types of display available
  /// </summary>
  public enum DotMatrix 
  {
    /// <summary>
    /// 5 pixels by 7 pixels matrix
    /// </summary>
    mat5x7,
    /// <summary>
    /// 5 pixels by 8 pixels matrix
    /// </summary>
    mat5x8,
    /// <summary>
    /// 7 pixels by 9 pixels matrix
    /// </summary>
    mat7x9,
    /// <summary>
    /// 9 pixels by 12 pixels matrix
    /// </summary>
    mat9x12,
    /// <summary>
    /// Hitachi style
    /// </summary>
    Hitachi,
    /// <summary>
    /// Hitachi 2 style
    /// </summary>
    Hitachi2,
    /// <summary>
    /// DOS style display
    /// </summary>
    dos5x7
  }

  /// <summary>
  /// Enumerated type defining the available LCD pixel sizes
  /// </summary>
  public enum PixelSize
  {
    /// <summary>
    /// 1 screen pixel is used for a LCD pixel 
    /// </summary>
    pix1x1,
    /// <summary>
    /// 2 screen pixels are used for a LCD pixel 
    /// </summary>
    pix2x2,
    /// <summary>
    /// 3 screen pixels are used for a LCD pixel
    /// </summary>
    pix3x3,
    /// <summary>
    /// 4 screen pixels are used for a LCD pixel
    /// </summary>
    pix4x4,
    /// <summary>
    /// 5 screen pixels are used for a LCD pixel
    /// </summary>
    pix5x5,
    /// <summary>
    /// 6 screen pixels are used for a LCD pixel
    /// </summary>
    pix6x6,
    /// <summary>
    /// 7 screen pixels are used for a LCD pixel
    /// </summary>
    pix7x7,
    /// <summary>
    /// 8 screen pixels are used for a LCD pixel
    /// </summary>
    pix8x8,
    /// <summary>
    /// 9 screen pixels are used for a LCD pixel
    /// </summary>
    pix9x9,
    /// <summary>
    /// 10 screen pixels are used for a LCD pixel
    /// </summary>
    pix10x10,
    /// <summary>
    /// 11 screen pixels are used for a LCD pixel
    /// </summary>
    pix11x11,
    /// <summary>
    /// 12 screen pixels are used for a LCD pixel
    /// </summary>
    pix12x12,
    /// <summary>
    /// 13 screen pixels are used for a LCD pixel
    /// </summary>
    pix13x13,
    /// <summary>
    /// 14 screen pixels are used for a LCD pixel
    /// </summary>
    pix14x14,
    /// <summary>
    /// 15 screen pixels are used for a LCD pixel
    /// </summary>
    pix15x15,
    /// <summary>
    /// 16 screen pixels are used for a LCD pixel
    /// </summary>
    pix16x16,
    /// <summary>
    /// A custom defined number of pixels are used for a LCD pixel
    /// </summary>
    pixCustom
  }

  /// <summary>
  /// Enumerated type defining how pixels will be drawn
  /// </summary>
  public enum PixelShape 
  {
    /// <summary>
    /// On and off pixels will be drawn as squares
    /// </summary>
    Square,
    /// <summary>
    /// On and off pixels will be drawn as circles
    /// </summary>
    Round,
    /// <summary>
    /// On pixels will be drawn as circles with faded edges, off pixels will be drawn as squares
    /// </summary>
    Shaped
  }

  /// <summary>
  /// Windows Form control that displays an LCD display
  /// </summary>
  public class LcdLabel : Control
  {
    /// <summary>
    /// Initializes a new instance of the <see cref="LcdLabel"/> class.
    /// </summary>
    public LcdLabel()
      : base()
    {
      DoubleBuffered = true;
      FWidth = 0;
      FHeight = 0;
      FCharSpacing = 2;
      FLineSpacing = 2;
      FPixelSpacing = 1;
      FBorderSpace = 3;
      FTextLines = 1;
      FNoOfChars = 10;
      FBorderColor = Color.Black;
      FBackGround = Color.Silver;
      FPixOnColor = Color.Black;
      FPixOffColor = Color.FromArgb(0x00AAAAAA);
      FPixelSize = PixelSize.pix2x2;
      CalcHalfColor();             // Halftone On color          
      CalcSize();                  // Get correct sizes at start
    }

    private PixelSize FPixelSize;   
    /// <summary>
    /// Gets or sets the size of a LCD pixel (in screen pixels)
    /// </summary>
    /// <value>The size of the pixel.</value>
    public PixelSize PixelSize
    {
      get { return FPixelSize; }
      set 
      {
        if (value != FPixelSize)
        {
          FPixelSize = value;
          Invalidate();
        }
      }
    }

    private PixelShape FPixelShape = PixelShape.Square;
    /// <summary>
    /// Gets or sets the shape of a LCD pixel
    /// </summary>
    /// <value>The pixel shape.</value>
    public PixelShape PixelShape 
    { 
      get { return FPixelShape; }
      set 
      { 
        if (value != FPixelShape)
        {
          FPixelShape = value;
          Invalidate();
        }
      }
    }

    private DotMatrix FDotMatrix = DotMatrix.mat5x7;
    /// <summary>
    /// Gets or sets the type of character matrix on the LCD
    /// </summary>
    /// <value>The type of character matrix on the LCD.</value>
    public DotMatrix DotMatrix 
    {
      get 
      {
        return FDotMatrix;
      }
      set
      {
        FDotMatrix = value;
        Invalidate();
      }
    }

    private int FPixelSpacing;
    /// <summary>
    /// Gets or sets the space between each pixel in the matrix.
    /// </summary>
    /// <value>The pixel spacing.</value>
    public int PixelSpacing
    {
      get { return FPixelSpacing; }
      set 
      {
        if (value < 0)
          throw new ArgumentException("Pixel spacing can't be less than zero");
        else 
        {
          if (value != FPixelSpacing)
          {
            FPixelSpacing = value;
            Invalidate();
          }
        }
      }
    }

    private int FCharSpacing;           
    /// <summary>
    /// Gets or sets the space between the characters on the LCD  
    /// </summary>
    /// <value>The character spacing.</value>
    public int CharSpacing
    {
      get { return FCharSpacing; }
      set 
      {  
        if (value < 0)
          throw new ArgumentException("Character spacing can't be less than zero");
        else 
        {
          if (value != FCharSpacing)
          {
            FCharSpacing = value;
            Invalidate();
          }
        }
      }
    }
    
    private int FLineSpacing;   
    /// <summary>
    /// Gets or sets the space between text lines on the display
    /// </summary>
    /// <value>The line spacing.</value>
    public int LineSpacing
    {
      get { return FLineSpacing; }
      set
      {
        if (value < 0)
          throw new ArgumentException("Line spacing can't be less than zero");
        else
        {
          if (value != FLineSpacing)
          {
            FLineSpacing = value;
            Invalidate();
          }
        }
      }
    }

    private int FBorderSpace;             
    /// <summary>
    /// Gets or sets the distance from the LCD border.
    /// </summary>
    /// <value>The distance from the LCD border.</value>
    public int BorderSpace
    {
      get { return FBorderSpace; }
      set
      {
        if (value < 0)     
          throw new ArgumentException("Border spacing can't be less than zero");
        else
        {
          if (value != FBorderSpace)
          {
            FBorderSpace = value;
            Invalidate();
          }
        }
      }
    }
    
    private int FTextLines;             
    /// <summary>
    /// Gets or sets the number of text lines on the LCD.
    /// </summary>
    /// <value>The number of text lines on the LCD.</value>
    public int TextLines
    {
      get { return FTextLines; }
      set
      {
        if (value < 1)
          throw new ArgumentException("Display needs at least one line");
        else
        {
          if (value != FTextLines)
          {
            FTextLines = value;
            Invalidate();
          }
        }
      }
    }

    private int FNoOfChars;           
    /// <summary>
    /// Gets or sets the number of characters on a single line.
    /// </summary>
    /// <value>The number of characters on a single line.</value>
    public int NumberOfCharacters 
    {
      get { return FNoOfChars; }
      set
      {
        if (value < 1)
          throw new ArgumentException("Display needs at least one character");
        else
        {
          if (value != FNoOfChars)
          {
            FNoOfChars = value;
            Invalidate();
          }
        }
      }
    }

    private Color FBackGround = Color.Silver;             
    /// <summary>
    /// Gets or sets the LCD background color.
    /// </summary>
    /// <value>The LCD background color.</value>
    public Color BackGround 
    {
      get { return FBackGround; }
      set
      {
        if(value != FBackGround)
        {
          FBackGround = value;
          Invalidate();
        }
      }
    }

    private Color FPixOnColor = Color.Black;                         
    /// <summary>
    /// Gets or sets the pixel on colour.
    /// </summary>
    /// <value>The pixel on colour.</value>
    public Color PixelOn
    {
      get { return FPixOnColor; }
      set
      {
        if (value != FPixOnColor)
        {
          FPixOnColor = value;
          CalcHalfColor();
          Invalidate();
        }
      }
    }

    private Color FPixOffColor = Color.Gray;                    
    /// <summary>
    /// Gets or sets the pixel off colour.
    /// </summary>
    /// <value>The pixel off colour.</value>
    public Color PixelOff 
    {
      get { return FPixOffColor; }
      set
      {
        if (value != FPixOffColor)
        {
          FPixOffColor = value;
          Invalidate();
        }
      }
    }

    private Color FPixHalfColor;          // Half intensity ON color                  }
    
    private int FPixWidth;            
    /// <summary>
    /// Gets or sets the width of a LCD pixel.
    /// </summary>
    /// <value>The width of a LCD pixel.</value>
    public int PixelWidth
    { 
      get { return FPixWidth; } 
      set 
      {
        if (FPixelSize == PixelSize.pixCustom)
        {
          if (value != FPixWidth)
          {
            if (value < 1)
              throw new ArgumentException("Display pixel width must be 1 or greater");
            else
            {
              FPixWidth = value;
              Invalidate();
            }
          }
        }
      }
    }
    
    private int FPixHeight;                                  
    /// <summary>
    /// Gets or sets the height of a LCD pixel.
    /// </summary>
    /// <value>The height of a LCD pixel.</value>
    public int PixelHeight
    {
      get { return FPixHeight; }
      set 
      {
        if (FPixelSize == PixelSize.pixCustom)
        {
          if (value != FPixHeight)
          {
            if (value < 1)
              throw new ArgumentException("Display pixel height must be 1 or greater");
            else
            {
              FPixHeight = value;
              Invalidate();
            }
          }
        }
      }
    }

    private Color FBorderColor = Color.Black;             
    /// <summary>
    /// Gets or sets the color of the border.
    /// </summary>
    /// <value>The color of the border.</value>
    public Color BorderColor
    {
      get { return FBorderColor; }
      set
      {
        if (value != FBorderColor)
        {
          FBorderColor = value;
          Invalidate();
        }
      }
    }

    private int FWidth;                   // Label width in pixels                    }
    private int FHeight;                  // Label height in pixels                   }
    private int charw, charh;             // Temp. storage of character sizes         }
    private int psx, psy;                 // Internal pixel width variables           }
    private int pix_x;
    private int pix_y;

    private int first_c, last_c;          // First and last character to draw         }

    /// <summary>
    /// Gets or sets the text displayed on the LCD display.
    /// </summary>
    /// <returns>
    /// The text displayed on the LCD display.
    /// </returns>
    public override string Text 
    {
      get 
      {
        return base.Text;
      }
      set
      {
        if (Text != value)
        {
          base.Text = value;
          Invalidate();  // Force a direct re-paint of label without any flicker
        }
      }
    }

    private void DrawMatrix(Graphics graphics, int xpos, int ypos, int charindex)
    {
      int tx, ty;
      Color CColor;
      tx = xpos;
      ty = ypos;

      charindex = charindex - first_c;

      using (SolidBrush brush = new SolidBrush(Color.Black))
      {
        for (int i = 0; i < pix_y; i++)
        {
          for (int j = 0; j < pix_x; j++)
          {
            CColor = FPixOffColor;
            switch (FDotMatrix)
            {
              case DotMatrix.mat5x7:
                if (Matrix.Char5x7[charindex, i, j] == 1)
                  CColor = FPixOnColor;
                else
                  CColor = FPixOffColor;
                break;
              case DotMatrix.mat5x8:
                if (Matrix.Char5x8[charindex, i, j] == 1)
                  CColor = FPixOnColor;
                else
                  CColor = FPixOffColor;
                break;
              case DotMatrix.Hitachi:
                if (Matrix.CharHitachi[charindex, i, j] == 1)
                  CColor = FPixOnColor;
                else
                  CColor = FPixOffColor;
                break;
              case DotMatrix.Hitachi2:
                // Use full height for character 194 - 223
                if (charindex <= 193)
                {  // Normal Hitachi
                  if (i < 7)
                  {
                    if (Matrix.CharHitachi[charindex, i, j] == 1)
                      CColor = FPixOnColor;
                    else
                      CColor = FPixOffColor;
                  }
                  else
                    CColor = FPixOffColor;
                }
                else
                {
                  // Extended height
                  if (Matrix.CharHitachiExt[charindex, i, j] == 1)
                    CColor = FPixOnColor;
                  else
                    CColor = FPixOffColor;
                }
                break;
              case DotMatrix.mat7x9:
                if (Matrix.Char7x9[charindex, i, j] == 1)
                  CColor = FPixOnColor;
                else
                  CColor = FPixOffColor;
                break;
              case DotMatrix.mat9x12:
                if (Matrix.Char9x12[charindex, i, j] == 1)
                  CColor = FPixOnColor;
                else
                  CColor = FPixOffColor;
                break;
              case DotMatrix.dos5x7:
                if (Matrix.CharDOS5x7[charindex, i, j] == 1)
                  CColor = FPixOnColor;
                else
                  CColor = FPixOffColor;
                break;
            }

            brush.Color = CColor;

            // Paint pixels in selected shape
            switch (FPixelShape)
            {
              case PixelShape.Square:
                graphics.FillRectangle(brush, tx, ty, psx, psy);
                break;
              case PixelShape.Round:
                graphics.FillEllipse(brush, tx, ty, psx, psy);
                break;
              case PixelShape.Shaped:
                if (CColor == FPixOnColor)
                {
                  brush.Color = FPixHalfColor;
                  graphics.FillRectangle(brush, tx, ty, psx, psy);
                  brush.Color = CColor;
                  graphics.FillEllipse(brush, tx, ty, psx, psy);
                }
                else
                {
                  brush.Color = CColor;
                  graphics.FillRectangle(brush, tx, ty, psx, psy);
                }
                break;
            }

            tx = tx + psx + FPixelSpacing;
          }
          tx = xpos;
          ty = ty + psy + FPixelSpacing;
        }
      }
    }

    private void DrawCharacters(Graphics graphics)
    {
      if (Text == null)
        return;

      int xpos, ypos;
      int charindex;
      int cc;
      bool textend;
  
      xpos = FBorderSpace + 1;
      ypos = FBorderSpace + 1;
      cc = 1;
      textend = false;
      for (int row=1; row<=FTextLines; row++)
      {
        // Line counter             }
        for (int col=1; col<=FNoOfChars; col++)
        {
          // Character counter        }
          if (!textend)              // Check for string end     }
            if (cc > Text.Length)
              textend = true;

          if (textend)
            charindex = 0;
          else
            charindex = Convert.ToInt32(Text[cc-1]);

          if (charindex < first_c)        // Limit charactes inside interval }
            charindex = first_c;
          
          if (charindex > last_c)
            charindex = last_c;

          DrawMatrix(graphics, xpos, ypos, charindex);
          xpos = xpos + charw + FCharSpacing;
          cc++;
        }
        ypos = ypos + charh + FLineSpacing;
        xpos = FBorderSpace + 1;
      }
    }

    //{ Calculate half color intensity }
    private void CalcHalfColor()
    {
      byte red, green, blue, control;
      blue = (byte)(FPixOnColor.B / 2);
      green = (byte)(FPixOnColor.G / 2);
      red = (byte)(FPixOnColor.R / 2);
      control = FPixOnColor.A;
      FPixHalfColor = Color.FromArgb(blue + (green * 0x100) + (red * 0x10000) + (control * 0x1000000));
    }

    //{ Calculations for width and height }
    private void CalcSize()
    {
      if (PixelSize.pixCustom == FPixelSize)  // Custom size }
      {  
        psx = FPixWidth;
        psy = FPixHeight;
      }
      else              // Predefined width selected - make square pixels }
      {
        psx = (int)FPixelSize + 1;
        psy = psx;
        FPixWidth = psx;
        FPixHeight = psy;
      }

      switch (FDotMatrix)         //{ Calculate the space taken by the character matrix }
      {
        case DotMatrix.mat5x7 :
        case DotMatrix.Hitachi :
          pix_x = Matrix.HITACHI_WIDTH;
          pix_y = Matrix.HITACHI_HEIGHT;
          break;
        case DotMatrix.Hitachi2 :
          pix_x = Matrix.HITACHI2_WIDTH;
          pix_y = Matrix.HITACHI2_HEIGHT;
          break;
        case DotMatrix.mat5x8 :
          pix_x = Matrix.MAT5X8_WIDTH;
          pix_y = Matrix.MAT5X8_HEIGHT;
          break;
        case DotMatrix.mat7x9 :
          pix_x = Matrix.MAT7X9_WIDTH;
          pix_y = Matrix.MAT7X9_HEIGHT;
          break;
        case DotMatrix.mat9x12:
          pix_x = Matrix.MAT9X12_WIDTH;
          pix_y = Matrix.MAT9X12_HEIGHT;
          break;
        case DotMatrix.dos5x7 :
          pix_x = Matrix.DOS5X7_WIDTH;
          pix_y = Matrix.DOS5X7_HEIGHT;
          break;    
      }

      charw = (pix_x * psx) + ((pix_x - 1) * FPixelSpacing);
      charh = (pix_y * psy) + ((pix_y - 1) * FPixelSpacing);
      Width = (FBorderSpace * 2) +                //{ Distance to sides (there are two) }
           (FCharSpacing * (FNoOfChars - 1)) + //{ Spaces between charactes          }
           (charw * FNoOfChars) +              //{ The characters itself             }
           2;                                  //{ Border outside character area     }
      Height = (FBorderSpace * 2) +                //{ Distance to top and bottom        }
           (FLineSpacing * (FTextLines - 1)) + //{ Spacing between lines             }
           (charh * FTextLines) +              //{ The lines                         }
           2;                                  //{ The Border                        }
      FWidth = Width;
      FHeight = Height;
    }

    //{ Get interval for ASCII values }
    private void GetAsciiInterval()
    {
      switch (FDotMatrix)
      {
        case DotMatrix.mat5x7 :
        case DotMatrix.Hitachi :
          first_c = Matrix.HITACHI_FIRST;
          last_c = Matrix.HITACHI_LAST;
          break;
        case DotMatrix.Hitachi2 :
          first_c = Matrix.HITACHI2_FIRST;
          last_c = Matrix.HITACHI2_LAST;
          break;
        case DotMatrix.mat5x8 :
          first_c = Matrix.MAT5X8_FIRST;
          last_c = Matrix.MAT5X8_LAST;
          break;
        case DotMatrix.mat7x9 :
          first_c = Matrix.MAT7X9_FIRST;
          last_c = Matrix.MAT7X9_LAST;
          break;
        case DotMatrix.mat9x12 :
          first_c = Matrix.MAT9X12_FIRST;
          last_c = Matrix.MAT9X12_LAST;
          break;
        case DotMatrix.dos5x7 :
          first_c = Matrix.DOS5X7_FIRST;
          last_c = Matrix.DOS5X7_LAST;
          break;
      }
    }

    //{ Calculate no of characters and lines from width and heigth }
    private void CalcCharSize()
    {
      if (PixelSize.pixCustom == FPixelSize) // Custom size
      {
        psx = FPixWidth;
        psy = FPixHeight;
      }
      else              // Predefined width selected - make square pixels
      {
        psx = (int)FPixelSize + 1;
        psy = psx;
        FPixWidth = psx;
        FPixHeight = psy;
      }

      switch(FDotMatrix)         //{ Calculate the space taken by the character matrix }
      {
        case DotMatrix.mat5x7 :
        case DotMatrix.Hitachi :
          pix_x = Matrix.HITACHI_WIDTH;
          pix_y = Matrix.HITACHI_HEIGHT;
          break;
        case DotMatrix.Hitachi2 :
          pix_x = Matrix.HITACHI2_WIDTH;
          pix_y = Matrix.HITACHI2_HEIGHT;
          break;
        case DotMatrix.mat5x8 :
          pix_x = Matrix.MAT5X8_WIDTH;
          pix_y = Matrix.MAT5X8_HEIGHT;
          break;
        case DotMatrix.mat7x9 :
          pix_x = Matrix.MAT7X9_WIDTH;
          pix_y = Matrix.MAT7X9_HEIGHT;
          break;
        case DotMatrix.mat9x12 :
          pix_x = Matrix.MAT9X12_WIDTH;
          pix_y = Matrix.MAT9X12_HEIGHT;
          break;
        case DotMatrix.dos5x7 :
          pix_x = Matrix.DOS5X7_WIDTH;
          pix_y = Matrix.DOS5X7_HEIGHT;
          break;
      }
  
      charw = (pix_x * psx) + ((pix_x - 1) * FPixelSpacing);
      charh = (pix_y * psy) + ((pix_y -1) * FPixelSpacing);
      FNoOfChars = (Width - (2 * FBorderSpace) + FCharSpacing) / (charw + FCharSpacing);
      FTextLines = (Height- (2 * FBorderSpace) + FLineSpacing) / (charh + FLineSpacing);
      if (FNoOfChars < 1)
        FNoOfChars = 1;
      if (FTextLines < 1)
        FTextLines = 1;
      Width = (FBorderSpace * 2) +                //{ Distance to sides (there are two) }
           (FCharSpacing * (FNoOfChars - 1)) + //{ Spaces between charactes          }
           (charw * FNoOfChars) +              //{ The characters itself             }
           2;                                  //{ For the border                    }
      Height = (FBorderSpace * 2) +                //{ Distance to top and bottom        }
           (FLineSpacing * (FTextLines - 1)) + //{ Spacing between lines             }
           (charh * FTextLines) +              //{ The lines                         }
           2;                                  //{ For the border                    }
      FWidth = Width;
      FHeight = Height;
    }

    /// <summary>
    /// Paints the controls and raises the <see cref="E:System.Windows.Forms.Control.Paint"/> event.
    /// </summary>
    /// <param name="e">A <see cref="T:System.Windows.Forms.PaintEventArgs"/> that contains the event data.</param>
    protected override void OnPaint(PaintEventArgs e) 
    {
      base.OnPaint(e);

      Rectangle T_Rect;
      bool flag = false;
  
      if (Width != FWidth)
      {
        flag = true;
        FWidth = Width;
      }

      if (Height != FHeight)
      {
        flag = true;
        FHeight = Height;
      }

      GetAsciiInterval();          // Calculate interval for ASCII values }
      if (flag)
        CalcCharSize();
      else
        CalcSize();                // Get Width and Height correct }

      using (SolidBrush brush = new SolidBrush(FBackGround))
      using (Pen pen = new Pen(brush))
      {
        T_Rect = new Rectangle(0, 0, Width, Height);
        // Border drawing
        if (Visible)     // Draw only real graphics if display is marked visible
        {  
          e.Graphics.FillRectangle(brush, T_Rect);          // Background fill 
          
          // Character drawing
          if (Enabled)
            DrawCharacters(e.Graphics);
        } 
        else  //{ Invisible - draw a square in background color }
        {
          brush.Color = SystemColors.ButtonFace;
          e.Graphics.FillRectangle(brush, T_Rect);
        }
      }
    }
  }
}
