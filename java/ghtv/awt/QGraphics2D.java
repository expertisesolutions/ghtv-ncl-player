/* (c) Copyright 2011-2014 Felipe Magno de Almeida
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

package ghtv.awt;

import java.awt.Graphics2D;
import java.awt.Graphics;
import java.awt.GraphicsConfiguration;
import java.awt.Composite;
import java.awt.Stroke;
import java.awt.Color;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Rectangle;
import java.awt.Shape;
import java.awt.Polygon;
import java.awt.Image;
import java.text.AttributedCharacterIterator;
import java.awt.image.ImageObserver;

public class QGraphics2D extends Graphics2D
{
    private static long bootstrap;
    private long peer;

    private native void nativeInit(long bootstrap);

    QGraphics2D()
    {
        nativeInit(bootstrap);
    }

    public Graphics create()
    {
        assert false;
        return null;
    }
    public void translate(int x, int y)
    {
        assert false;
    }
    public Color getColor()
    {
        assert false;
        return null;
    }
    public void setColor(Color c)
    {
    }
    public void setPaintMode()
    {
        assert false;
    }
    public void setXORMode(Color c1)
    {
        assert false;
    }
    public Font getFont()
    {
        return null;
    }
    public void setFont(Font font)
    {
    }
    public FontMetrics getFontMetrics(Font f)
    {
        assert false;
        return null;
    }
    public Rectangle getClipBounds()
    {
        assert false;
        return null;
    }
    public void clipRect(int x, int y, int width, int height)
    {
        assert false;
    }
    private native void nativeSetClip(long peer, int x, int y, int width, int height);
    public void setClip(int x, int y, int width, int height)
    {
        assert peer != 0;
        nativeSetClip(peer, x, y, width, height);
    }
    private native Shape nativeGetClip(long peer);
    public Shape getClip()
    {
        return nativeGetClip(peer);
    }
    public void setClip(Shape clip)
    {
        assert false;
    }
    public void copyArea(int x, int y, int width, int heigh
                         , int dx, int dy)
    {
        assert false;
    }
    public void drawLine(int x1, int y1, int x2, int y2)
    {
        assert false;
    }
    public void fillRect(int x, int y, int width, int height)
    {
        assert false;
    }
    public void clearRect(int x, int y, int width, int height)
    {
        assert false;
    }
    public void drawRoundRect(int x, int y, int width, int height
                              , int arcWidth, int arcHeight)
    {
        assert false;
    }
    public void fillRoundRect(int x, int y, int width, int height
                              , int arcWidth, int arcHeight)
    {
        assert false;
    }
    public void drawOval(int x, int y, int width, int height)
    {
        assert false;
    }
    public void fillOval(int x, int y, int width, int height)
    {
        assert false;
    }
    public void drawArc(int x, int y, int width, int height
                        , int startAngle, int arcAngle)
    {
        assert false;
    }
    public void fillArc(int x, int y, int width, int height
                        , int startAngle, int arcAngle)
    {
        assert false;
    }
    public void drawPolyline(int[] xPoints, int[] yPoints, int nPoints)
    {
        assert false;
    }
    public void drawPolygon(int[] xPoints, int[] yPoints, int nPoints)
    {
        assert false;
    }
    public void drawPolygon(Polygon p)
    {
        assert false;
    }
    public void fillPolygon(int[] xPoints, int[] yPoints, int nPoints)
    {
        assert false;
    }
    public void fillPolygon(Polygon p)
    {
        assert false;
    }
    public void drawString(AttributedCharacterIterator iterator, int x, int y)
    {
        assert false;
    }
    private native boolean nativeDrawImage(long peer, Image img, int x, int y, ImageObserver observer);
    public boolean drawImage(Image img, int x, int y, ImageObserver observer)
    {
        assert peer != 0;
        assert img != null;
        return nativeDrawImage(peer, img, x, y, observer);
    }
    public boolean drawImage(Image img, int x, int y, int width, int height
                             , ImageObserver observer)
    {
        assert false;
        return false;
    }
    public boolean drawImage(Image img, int x, int y, Color bgcolor, ImageObserver observer)
    {
        assert false;
        return false;
    }
    public boolean drawImage(Image img, int x, int y, int width, int height, Color bgcolor, ImageObserver observer)
    {
        assert false;
        return false;
    }
    public boolean drawImage(Image img, int dx1, int dy1, int dx2, int dy2, int sx1, int sy1, int sx2, int sy2, ImageObserver observer)
    {
        return drawImage(img, dx1, dy1, observer);
    }
    public boolean drawImage(Image img, int dx1, int dy1, int dx2, int dy2, int sx1, int sy1, int sx2, int sy2, Color bgcolor, ImageObserver observer)
    {
        assert false;
        return false;
    }
    private native void nativeDispose(long peer);
    public void dispose()
    {
        assert false;
    }
    private native void nativeDrawString(long peer, String str, int x, int y);
    public void drawString(String str, int x, int y)
    {
        nativeDrawString(peer, str, x, y);
    }
    public GraphicsConfiguration getDeviceConfiguration()
    {
        assert false;
        return null;
    }
    public void setComposite(Composite comp)
    {
        assert false;
    }
    public void setStroke(Stroke s)
    {
        assert false;
    }
    public Composite getComposite()
    {
        assert false;
        return null;
    }
    public Stroke getStroke()
    {
        assert false;
        return null;
    }
}
