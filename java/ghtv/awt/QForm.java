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
import com.sun.dtv.lwuit.animations.Animation;
import com.sun.dtv.lwuit.Form;

public class QForm extends java.awt.Container
{
    private static long bootstrap;
    private long peer;

    private native void nativeInit(long bootstrap);
    private Form form;

    public QForm()
    {
        nativeInit(bootstrap);
        assert(peer != 0);
    }

    public void initForm(Form form)
    {
        assert form != null;
        assert this.form == null;
        this.form = form;
    }

    public void paint(java.awt.Graphics g)
    {
        java.awt.Graphics2D g2d = (java.awt.Graphics2D)g;
        form.paint(new com.sun.dtv.lwuit.Graphics(g2d));
    }

    private native void nativeRegisterAnimation(long peer, Animation a);
    public void registerAnimation(Animation a)
    {
        System.out.println("QForm::registerAnimation");
        nativeRegisterAnimation(peer, a);
        try
        {
            Component c = (Component)a;
            c.setVisible(true);
        }
        catch(ClassCastException e)
        {}
    }
    private native void nativeDeregisterAnimation(long peer, Animation a);
    public void deregisterAnimation(Animation a)
    {
        System.out.println("QForm::deregisterAnimation");
        nativeDeregisterAnimation(peer, a);
        try
        {
            Component c = (Component)a;
            c.setVisible(false);
        }
        catch(ClassCastException e)
        {}
    }
    private native void nativeShow(long peer);
    public void show()
    {
        assert(peer != 0);
        nativeShow(peer);
    }

    public void longKeyPress(int keyCode)
    {
        form.longKeyPress(keyCode);
    }
    public void keyPressed(int keyCode)
    {
        form.keyPressed(keyCode);
    }
    public void keyReleased(int keyCode)
    {
        form.keyReleased(keyCode);
    }
    public void keyRepeated(int keyCode)
    {
        form.keyRepeated(keyCode);
    }
    public void pointerPressed(int x, int y)
    {
        form.pointerPressed(x, y);
    }
    public void pointerDragged(int x, int y)
    {
        form.pointerDragged(x, y);
    }
}
