import uproot
import awkward as ak
import numpy as np


# Util functions

def GetCoordinates(file, bname):
    x = file['ana/tree']["{0}fCoordinates.fX".format(bname)].array()
    y = file['ana/tree']["{0}fCoordinates.fY".format(bname)].array()
    z = file['ana/tree']["{0}fCoordinates.fZ".format(bname)].array()
    
    return x, y, z
    

def ReadBranch(file, bname):
    coords = [
        "trackPoints",
        "spacePoints",
        "startDir",
        "endDir",
        "trackStart",
        "trackEnd"
    ]
    
    if bname.split('/')[0] in coords:
        return GetCoordinates(file, bname)
        
    return file['ana/tree'][bname].array()
    

def GetTPs(file):
    """
    Retrieve fitted track points
    """
    x, y, z = ReadBranch(file, "trackPoints/trackPoints.")

    x = ak.drop_none(x[x > -990])
    y = ak.drop_none(y[y > -990])
    z = ak.drop_none(z[z > -990])

    return x, y, z


def GetSPs(file):
    """
    Retrieve space points
    """
    x, y, z = ReadBranch(file, 'spacePoints/spacePoints.')

    x = ak.drop_none(x[x > -990])
    y = ak.drop_none(y[y > -990])
    z = ak.drop_none(z[z > -990])

    return x, y, z


def GetDir(file, bname):
    x, y, z = ReadBranch(file, "{0}/fCoordinates/".format(bname))

    x = ak.drop_none(x[np.abs(x) <= 1])
    y = ak.drop_none(y[np.abs(x) <= 1])
    z = ak.drop_none(z[np.abs(x) <= 1])

    return x, y, z

def CreateSlider(nTracks, nTraces):
    """
    Create a slider for visualization
    """
    steps = []
    for track in range(nTracks*nTraces, nTraces):
        step = dict(
            method="update",
            args=[{"visible": [False] * nTracks * nTraces}],  
        )

        for trace in range(nTraces):
            step["args"][0]["visible"][track + trace] = True
        
        steps.append(step)

    sliders = [dict(
        active=nTraces,
        currentvalue={"prefix": "Frequency: "},
        pad={"t": 50},
        steps=steps
    )]

    return sliders