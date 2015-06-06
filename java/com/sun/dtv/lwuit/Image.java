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

package com.sun.dtv.lwuit;

import java.awt.Color;
import java.io.IOException;
import java.io.InputStream;
import ghtv.awt.QImage;

public class Image
{
    public Image(java.awt.Image image)
    {
        awtImage = (QImage)image;
    }
    public Image subImage(int x, int y, int width, int height, boolean processAlpha)
    {
        return new Image(awtImage.imageClone());
    }
    public Image rotate(int degrees)
    {
        assert false;
        return null;
    }
    public Image modifyAlpha(byte alpha)
    {
        assert false;
        return null;
    }
    public Image modifyAlpha(byte alpha, Color removeColor)
    {
        assert false;
        return null;
    }
    public static Image createImage(String path) throws IOException
    {
        return new Image(new QImage(path));
    }
    public static Image createImage(InputStream stream) throws IOException
    {
        assert false;
        return null;
    }
    public static Image createImage(int[] rgb, int width, int height)
    {
        assert false;
        return null;
    }
    public static Image createImage(byte[] bytes, int offset, int len)
    {
        assert false;
        return null;
    }
    public static Image createImage(java.awt.Image image)
    {
        assert false;
        return null;
    }
    public Graphics getGraphics()
    {
        assert false;
        return null;
    }
    public int getWidth()
    {
        return awtImage.getWidth(null);
    }
    public int getHeight()
    {
        return awtImage.getHeight(null);
    }
    public int[] getRGB()
    {
        assert false;
        return null;
    }
    public Image scaledWidth(int width)
    {
        return new Image(awtImage.getScaledWidthInstance(width));
    }
    public Image scaledHeight(int height)
    {
        return new Image(awtImage.getScaledHeightInstance(height));
    }
    public Image scaledSmallerRatio(int width, int height)
    {
        assert false;
        return null;
    }
    public Image scaled(int width, int height)
    {
        return new Image(awtImage.getScaledInstance(width, height, 0));
    }
    public java.awt.Image getAWTImage()
    {
        return awtImage;
    }
    public boolean isAnimation()
    {
        assert false;
        return false;
    }
    public boolean isOpaque()
    {
        assert false;
        return false;
    }

    private QImage awtImage;
}
