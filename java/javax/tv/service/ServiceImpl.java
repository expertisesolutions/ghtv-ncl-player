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

package javax.tv.service;

import javax.tv.locator.Locator;
import javax.tv.locator.LocatorImpl;

public class ServiceImpl implements Service
{
    public SIRequest retrieveDetails(SIRequestor requestor)
    {
        assert false;
        return null;
    }
    public String getName()
    {
        assert false;
        return null;
    }
    public boolean hasMultipleInstances()
    {
        assert false;
        return false;
    }
    public ServiceType getServiceType()
    {
        assert false;
        return null;
    }
    public Locator getLocator()
    {
        return new LocatorImpl();
    }
    public boolean equals(Object obj)
    {
        assert false;
        return false;
    }
    public int hashCode()
    {
        assert false;
        return 0;
    }
}
