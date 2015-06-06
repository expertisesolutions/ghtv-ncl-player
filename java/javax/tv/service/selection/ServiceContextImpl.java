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

import javax.tv.service.Service;
import javax.tv.service.ServiceImpl;
import javax.tv.locator.Locator;
import javax.tv.locator.InvalidLocatorException;

class ServiceContextImpl implements ServiceContext
{
    public void select(Service selection) throws SecurityException
    {
        assert false;
    }
    public void select(Locator[] components)
            throws InvalidLocatorException,
                   InvalidServiceComponentException, SecurityException
    {
        assert false;
    }
    public void stop() throws SecurityException
    {
        assert false;
    }
    public void destroy() throws SecurityException
    {
        assert false;
    }
    public ServiceContentHandler[] getServiceContentHandlers() throws SecurityException
    {
        return new ServiceContentHandler[]{new VideoServiceContentHandler()};
    }
    public Service getService()
    {
        return new ServiceImpl();
    }
    public void addListener(ServiceContextListener listener)
    {
        assert false;
    }
    public void removeListener(ServiceContextListener listener)
    {
        assert false;
    }
}
