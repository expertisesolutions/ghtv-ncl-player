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

package ghtv.classloader;

import java.security.Permission;
import java.io.FileDescriptor;
import java.net.InetAddress;

class SecurityManager extends java.lang.SecurityManager
{
    // public boolean getInCheck();
    // protected Class[] getClassContext();
    // protected ClassLoader currentClassLoader();
    // protected int classDepth(String name);
    // protected int classLoaderDepth();
    // protected boolean inClass(String name);
    // protected boolean inClassLoader();
    // public Object getSecurityContext();
    public void checkPermission(Permission perm)
    {
    }
    public void checkPermission(Permission perm, Object context)
    {
    }
    public void checkCreateClassLoader()
    {
    }
    public void checkAccess(Thread t)
    {
    }
    public void checkAccess(ThreadGroup g)
    {
    }
    public void checkExit(int status)
    {
        assert false;
    }
    public void checkExec(String cmd)
    {
        assert false;
    }
    public void checkLink(String li)
    {
        System.out.println("Trying to open link " + li);
    }
    public void checkRead(FileDescriptor fd)
    {
        //assert false;
    }
    public void checkRead(String file)
    {
        //assert false;
    }
    public void checkRead(String file, Object context)
    {
        //assert false;
    }
    public void checkWrite(FileDescriptor fd)
    {
        assert false;
    }
    public void checkWrite(String file)
    {
        assert false;
    }
    public void checkDelete(String file)
    {
        assert false;
    }
    public void checkConnect(String host, int port)
    {
    }
    public void checkConnect(String host, int port, Object context)
    {
    }
    public void checkListen(int port)
    {
        assert false;
    }
    public void checkAccept(String host, int port)
    {
        assert false;
    }
    public void checkMulticast(InetAddress maddr)
    {
        assert false;
    }
    public void checkMulticast(InetAddress maddr, byte ttl)
    {
        assert false;
    }
    public void checkPropertiesAccess()
    {
    }
    public void checkPropertyAccess(String key)
    {
    }
    // public boolean checkTopLevelWindow(Object window);
    public void checkPrintJobAccess()
    {
        assert false;
    }
    public void checkSystemClipboardAccess()
    {
        assert false;
    }
    public void checkAwtEventQueueAccess()
    {
        assert false;
    }
    public void checkPackageAccess(String pkg)
    {
    }
    public void checkPackageDefinition(String pkg)
    {
        assert false;
    }
    public void checkSetFactory()
    {
        assert false;
    }
    public void checkMemberAccess(Class clazz, int which)
    {
    }
    public void checkSecurityAccess(String target)
    {
    }
    // public ThreadGroup getThreadGroup();
}
