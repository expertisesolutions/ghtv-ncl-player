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

package ghtv.debug;

import java.io.IOException;

public class ErrorWriter extends java.io.Writer
{
    private static long bootstrap;
    private long peer;

    private native void nativeInit(long bootstrap);

    public ErrorWriter()
    {
        nativeInit(bootstrap);
    }

    private native void nativeWrite(long peer, char[] c, int off, int len)
        throws IOException;
    public void write(char[] c, int off, int len) throws IOException
    {
        nativeWrite(peer, c, off, len);
    }

    private native void nativeClose(long peer)
        throws IOException;
    public void close() throws IOException
    {
        nativeClose(peer);
    }

    private native void nativeFlush(long peer)
        throws IOException;
    public void flush() throws IOException
    {
        nativeFlush(peer);
    }
}