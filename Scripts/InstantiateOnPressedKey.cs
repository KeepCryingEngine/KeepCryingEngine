using KeepCryingEngine;

class InstantiateOnPressedKey: Component
{
    public float x, y, z;

    public override void Update()
    {
        if (Input.GetKey(KeyCode.A) == KeyState.KEY_DOWN)
        {
            Debug.Log(" A Pressed");
            InstanceManager.InstantiateCube(x,y,z);
        }
    }
}
