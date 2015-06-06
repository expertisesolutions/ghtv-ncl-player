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

package javax.tv.service.selection;

import javax.tv.locator.Locator;
import javax.tv.locator.LocatorImpl;
import javax.media.Player;
import javax.media.GainControl;
import javax.media.Controller;
import javax.media.IncompatibleTimeBaseException;
import javax.media.IncompatibleSourceException;
import javax.media.Time;
import javax.media.Control;
import javax.media.ControllerListener;
import javax.media.TimeBase;
import javax.media.ClockStoppedException;
import javax.media.protocol.DataSource;
import java.io.IOException;

public class VideoServiceContentHandler extends java.awt.Component
    implements ServiceContentHandler, Player
{
    public Locator[] getServiceContentLocators()
    {
        return new Locator[]{new LocatorImpl()};
    }

    public java.awt.Component getVisualComponent()
    {
        return this;
    }
    public GainControl getGainControl()
    {
        assert false;
        return null;
    }
    public java.awt.Component getControlPanelComponent()
    {
        assert false;
        return null;
    }

    public void start()
    {
        assert false;
    }
    
    public void addController(Controller newController) 
        throws IncompatibleTimeBaseException
    {
        assert false;
    }
    public void removeController(Controller oldController)
    {
        assert false;
    }
    public void setSource(DataSource source) throws IOException, IncompatibleSourceException
    {
        assert false;
    }
    public int getState()
    {
        return Controller.Started;
    }
    public int getTargetState()
    {
        assert false;
        return 0;
    }
    public void realize()
    {
        assert false;
    }
    public void prefetch()
    {
        assert false;
    }
    public void deallocate()
    {
        assert false;
    }
    public void close()
    {
        assert false;
    }
    public Time getStartLatency()
    {
        assert false;
        return null;
    }
    public Control[] getControls()
    {
        assert false;
        return null;
    }
    public Control getControl(String forName)
    {
        System.out.println("Getting control for " + forName);
        //assert false;
        return null;
    }
    public void addControllerListener(ControllerListener listener)
    {
        assert false;
    }
    public void removeControllerListener(ControllerListener listener)
    {
        assert false;
    }

    public Time getDuration()
    {
        assert false;
        return null;
    }

    public void setTimeBase(TimeBase master) throws IncompatibleTimeBaseException
    {
        assert false;
    }
    public void syncStart(Time at)
    {
        assert false;
    }
    public void stop()
    {
        assert false;
    }
    public void setStopTime()
    {
        assert false;
    }
    public Time getStopTime()
    {
        assert false;
        return null;
    }
    public void setMediaTime(Time now)
    {
        assert false;
    }
    public Time getMediaTime()
    {
        assert false;
        return null;
    }
    public long getMediaNanoseconds()
    {
        assert false;
        return 0;
    }
    public Time getSyncTime()
    {
        assert false;
        return null;
    }
    public TimeBase getTimeBase()
    {
        assert false;
        return null;
    }
    public Time mapToTimeBase(Time t) throws ClockStoppedException
    {
        assert false;
        return null;
    }
    public float getRate()
    {
        assert false;
        return 0.0f;
    }
    public float setRate(float factor)
    {
        assert false;
        return 0.0f;
    }
}
