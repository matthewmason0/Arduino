Public Class Form1
    Dim WaveR As New NAudio.Wave.WaveOut
    Dim Rsig() As Byte = IO.File.ReadAllBytes("C:\Users\Matthew\GDriveBU\Documents\Arduino\Robot Control VB Files\Robot Control\Rsig.wav")
    Dim stopWatchR As New Stopwatch()
    Dim newClick As Boolean = True
    Dim PadPos As New Point(29, 29)
    Dim LastPoint As New Point
    Sub PlaySoundR(ByVal vol As Decimal)
        Dim data As New IO.MemoryStream(Rsig) 'Data stream for the buffer
        WaveR.Init(New NAudio.Wave.BlockAlignReductionStream(NAudio.Wave.WaveFormatConversionStream.CreatePcmStream(New NAudio.Wave.WaveFileReader(data))))
        WaveR.Volume = vol
        WaveR.Play()
        stopWatchR.Restart()
    End Sub
    Function CalcRvol(ByVal _x As Integer, _y As Integer) As Decimal
        Dim Rspeed As Integer
        Dim Rvol As Decimal
        If _x > 0 Then
            Rspeed = _y - _x * _y / 50
        ElseIf _x < 0 Then
            Rspeed = _y
        Else
            Rspeed = _y
        End If
        If Rspeed > 0 Then
            Rvol = Rspeed / 200.0 + 0.5
        ElseIf Rspeed < 0 Then
            Rvol = Rspeed / -200.0
        Else
            Rvol = 0
        End If
        Return Rvol
    End Function
    Sub UpdateMotors(ByVal _x As Integer, ByVal _y As Integer)
        WaveR.Volume = CalcRvol(_x, _y)
    End Sub
    Sub StartMotors(ByVal _x As Integer, ByVal _y As Integer)
        PlaySoundR(CalcRvol(_x, _y))
    End Sub
    Sub StopMotors()
        WaveR.Stop()
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
