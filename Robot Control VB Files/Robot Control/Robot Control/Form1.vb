Public Class Form1
    Dim WaveL As New NAudio.Wave.WaveOut
    Dim Lsig() As Byte = IO.File.ReadAllBytes("C:\Users\Matthew\GDriveBU\Documents\Arduino\Robot Control VB Files\Robot Control\Lsig.wav")
    Dim stopWatchL As New Stopwatch()
    Dim newClick As Boolean = True
    Dim PadPos As New Point(29, 29)
    Dim LastPoint As New Point
    Public Sub PlaySoundL(ByVal vol As Decimal)
        Dim data As New IO.MemoryStream(Lsig) 'Data stream for the buffer
        WaveL.Init(New NAudio.Wave.BlockAlignReductionStream(NAudio.Wave.WaveFormatConversionStream.CreatePcmStream(New NAudio.Wave.WaveFileReader(data))))
        WaveL.Volume = vol
        WaveL.Play()
        stopWatchL.Restart()
    End Sub
    Function CalcLvol(ByVal _x As Integer, _y As Integer) As Decimal
        Dim Lspeed As Integer
        Dim Lvol As Decimal
        If _x > 0 Then
            Lspeed = _y
        ElseIf _x < 0 Then
            Lspeed = _y + _x * _y / 50
        Else
            Lspeed = _y
        End If
        If Lspeed > 0 Then
            Lvol = Lspeed / 200.0 + 0.5
        ElseIf Lspeed < 0 Then
            Lvol = Lspeed / -200.0
        Else
            Lvol = 0
        End If
        Return Lvol
    End Function
    Sub UpdateMotors(ByVal _x As Integer, ByVal _y As Integer)
        WaveL.Volume = CalcLvol(_x, _y)
    End Sub
    Sub StartMotors(ByVal _x As Integer, ByVal _y As Integer)
        PlaySoundL(CalcLvol(_x, _y))
    End Sub
    Sub StopMotors()
        WaveL.Stop()
    End Sub
    Private Sub StartButton_Click(sender As Object, e As EventArgs) Handles StartButton.Click
        MainTimer.Start()
    End Sub
    Private Sub MainTimer_Tick(sender As Object, e As EventArgs) Handles MainTimer.Tick
        If MouseButtons = Windows.Forms.MouseButtons.Left Then
            If LastPoint <> Windows.Forms.Cursor.Position Or newClick = True Then
                If Windows.Forms.Cursor.Position.X - PadPos.X <= 200 AndAlso Windows.Forms.Cursor.Position.Y - PadPos.Y <= 200 AndAlso Windows.Forms.Cursor.Position.X >= PadPos.X AndAlso Windows.Forms.Cursor.Position.Y >= PadPos.Y Then
                    Dim x As Integer = Windows.Forms.Cursor.Position.X - PadPos.X - 100
                    Dim y As Integer = 100 - Windows.Forms.Cursor.Position.Y + PadPos.Y
                    UpdateMotors(x, y)
                    If newClick = True Then
                        StartMotors(x, y)
                        newClick = False
                    End If
                End If
            End If
        Else
            If newClick = False Then
                StopMotors()
                newClick = True
            End If
        End If
        LastPoint = Windows.Forms.Cursor.Position
    End Sub
    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        LastPoint = Windows.Forms.Cursor.Position
    End Sub
End Class
