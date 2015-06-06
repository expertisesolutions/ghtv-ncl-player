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
import java.awt.Container;
import java.awt.Component;

public class LwuitComponent extends Container
{
    private com.sun.dtv.lwuit.Component lwuit_component;

    public LwuitComponent(com.sun.dtv.lwuit.Component c)
    {
        assert c != null;
        lwuit_component = c;
    }

    public void paint(java.awt.Graphics g)
    {
        java.awt.Graphics2D g2d = (java.awt.Graphics2D)g;
        lwuit_component.paint(new com.sun.dtv.lwuit.Graphics(g2d));
    }

    // public void longKeyPress(int keyCode)
    // {
    //     form.longKeyPress(keyCode);
    // }
    // public void keyPressed(int keyCode)
    // {
    //     form.keyPressed(keyCode);
    // }
    // public void keyReleased(int keyCode)
    // {
    //     form.keyReleased(keyCode);
    // }
    // public void keyRepeated(int keyCode)
    // {
    //     form.keyRepeated(keyCode);
    // }
    // public void pointerPressed(int x, int y)
    // {
    //     form.pointerPressed(x, y);
    // }
    // public void pointerDragged(int x, int y)
    // {
    //     form.pointerDragged(x, y);
    // }
}
