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

import java.awt.image.ImageObserver;
import java.awt.image.ImageProducer;
import java.awt.Graphics;
import java.awt.Image;

public class QImage extends Image
{
    private static long bootstrap;
    private long peer;

    private native void nativeInit(long bootstrap);
    private native void nativeConstructor(long peer, String path);

    public QImage(String path)
    {
        nativeInit(bootstrap);
        nativeConstructor(peer, path);
    }

    private native int nativeGetWidth(long peer);
    public int getWidth(ImageObserver observer)
    {
        return nativeGetWidth(peer);
    }
    private native int nativeGetHeight(long peer);
    public int getHeight(ImageObserver observer)
    {
        return nativeGetHeight(peer);
    }
    public ImageProducer getSource()
    {
        assert false;
        return null;
    }
    public Graphics getGraphics()
    {
        assert false;
        return null;
    }
    public Object getProperty(String name, ImageObserver observer)
    {
        assert false;
        return null;
    }
    public QImage imageClone()
    {
        return new QImage(nativePath(peer));
    }
    private native String nativePath(long peer);
    private native void nativeSetScaling(long peer, int width, int height);
    private native void nativeSetScalingWidth(long peer, int width);
    private native void nativeSetScalingHeight(long peer, int height);
    public QImage getScaledInstance(int width, int height, int hints)
    {
        QImage r = imageClone();
        r.nativeSetScaling(peer, width, height);
        return r;
    }
    public QImage getScaledWidthInstance(int width)
    {
        QImage r = imageClone();
        r.nativeSetScalingWidth(peer, width);
        return r;
    }
    public Image getScaledHeightInstance(int height)
    {
        QImage r = imageClone();
        r.nativeSetScalingHeight(peer, height);
        return r;
    }
    public void flush()
    {
    }
}
