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

import com.sun.dtv.lwuit.plaf.Style;
import com.sun.dtv.lwuit.events.ActionListener;
import com.sun.dtv.lwuit.animations.Transition;
import com.sun.dtv.lwuit.animations.Animation;
import com.sun.dtv.lwuit.list.ListCellRenderer;
import java.awt.Color;
import ghtv.awt.QForm;

public class Form extends Container
{
    private QForm form;

    public Form()
    {
        super(new QForm());
        form = (QForm)getAWTComponent();
        form.initForm(this);
    }
    public Form(String title)
    {
        assert false;
    }
    public void setSoftButtonStyle(Style s)
    {
        assert false;
    }
    public Style getSoftButtonStyle()
    {
        assert false;
        return null;
    }
    public void setGlassPane(Painter glassPane)
    {
        assert false;
    }
    public Painter getGlassPane()
    {
        assert false;
        return null;
    }
    public void setTitleStyle(Style s)
    {
        assert false;
    }
    public Label getTitleComponent()
    {
        assert false;
        return null;
    }
    public void addKeyListener(int keyCode, ActionListener listener)
    {
        assert false;
    }
    public void removeKeyListener(int keyCode, ActionListener listener)
    {
        assert false;
    }
    public void removeGameKeyListener(int keyCode, ActionListener listener)
    {
        assert false;
    }
    public void addGameKeyListener(int keyCode, ActionListener listener)
    {
        assert false;
    }
    public int getSoftButtonCount()
    {
        assert false;
        return 0;
    }
    public Button getSoftButton(int offset)
    {
        assert false;
        return null;
    }
    public Style getTitleStyle()
    {
        assert false;
        return null;
    }
    public void setDefaultCommand(Command defaultCommand)
    {
        assert false;
    }
    public Command getDefaultCommand()
    {
        assert false;
        return null;
    }
    public void setClearCommand(Command clearCommand)
    {
        assert false;
    }
    public Command getClearCommand()
    {
        assert false;
        return null;
    }
    public void setBackCommand(Command backCommand)
    {
        assert false;
    }
    public Command getBackCommand()
    {
        assert false;
        return null;
    }
    public Container getContentPane()
    {
        assert false;
        return null;
    }
    public void removeAll()
    {
        //assert false;
    }
    public void setBgImage(Image bgImage)
    {
        //assert false;
    }
    public void setTitle(String title)
    {
        assert false;
    }
    public String getTitle()
    {
        assert false;
        return null;
    }
    public void addComponent(Object constraints, Component cmp)
    {
        assert false;
        assert false;
    }
    public void addComponent(int index, Component cmp)
    {
        assert false;
        assert false;
    }
    public void replace(Component current, Component next, Transition t)
    {
        assert false;
        assert false;
    }
    public void removeComponent(Component cmp)
    {
        assert false;
        assert false;
    }
    public void registerAnimation(Animation a)
    {
        form.registerAnimation(a);
    }
    public void deregisterAnimation(Animation a)
    {
        form.deregisterAnimation(a);
    }
    public void refreshTheme()
    {
        assert false;
    }
    public void paintBackground(Graphics g)
    {
        assert false;
    }
    public Transition getTransitionInAnimator()
    {
        assert false;
        return null;
    }
    public void setTransitionInAnimator(Transition transitionInAnimator)
    {
        assert false;
    }
    public Transition getTransitionOutAnimator()
    {
        assert false;
        return null;
    }
    public void setCommandListener(ActionListener commandListener)
    {
        assert false;
    }
    public void show()
    {
        form.show();
    }
    public void setSmoothScrolling(boolean smoothScrolling)
    {
        assert false;
    }
    public boolean isSmoothScrolling()
    {
        assert false;
        return false;
    }
    public int getScrollAnimationSpeed()
    {
        assert false;
        return 0;
    }
    public void setScrollAnimationSpeed(int animationSpeed)
    {
        assert false;
    }
    public final Form getComponentForm()
    {
        return this;
    }
    public void setFocused(Component focused)
    {
        //assert false;
    }
    public Component getFocused()
    {
        assert false;
        return null;
    }
    public void longKeyPress(int keyCode)
    {
        assert false;
    }
    public void keyPressed(int keyCode)
    {
        assert false;
    }
    public void keyReleased(int keyCode)
    {
        assert false;
    }
    public void keyRepeated(int keyCode)
    {
        assert false;
    }
    public void pointerPressed(int x, int y)
    {
        assert false;
    }
    public void pointerDragged(int x, int y)
    {
        assert false;
    }
    public boolean isSingleFocusMode()
    {
        assert false;
        return false;
    }
    public void pointerReleased(int x, int y)
    {
        assert false;
    }
    public void setScrollableY(boolean scrollableY)
    {
        assert false;
    }
    public void setScrollableX(boolean scrollableX)
    {
        assert false;
    }
    public void addCommand(Command cmd, int offset)
    {
        assert false;
    }
    public int getCommandCount()
    {
        assert false;
        return 0;
    }
    public Command getCommand(int index)
    {
        assert false;
        return null;
    }
    public void addCommand(Command cmd)
    {
        assert false;
    }
    public void removeCommand(Command cmd)
    {
        assert false;
    }
    public void scrollComponentToVisible(Component c)
    {
        assert false;
    }
    public void setMenuCellRenderer(ListCellRenderer menuCellRenderer)
    {
        assert false;
    }
    public void removeAllCommands()
    {
        assert false;
    }
    public void paint(Graphics g)
    {
        //assert false;
    }
    public void setScrollable(boolean scrollable)
    {
        assert false;
    }
    public void setVisible(boolean visible)
    {
        assert false;
    }
    public Color getTintColor()
    {
        assert false;
        return null;
    }
    public void setTintColor(Color tintColor)
    {
        assert false;
    }
    public void setMenuTransitions(Transition transitionIn, Transition transitionOut)
    {
        assert false;
    }
    protected void onShow()
    {
        assert false;
    }
    protected void sizeChanged(int w, int h)
    {
        assert false;
    }
    protected void hideNotify()
    {
        assert false;
    }
    protected void showNotify()
    {
        assert false;
    }
    protected Command showMenuDialog(Dialog menu)
    {
        assert false;
        return null;
    }
}
