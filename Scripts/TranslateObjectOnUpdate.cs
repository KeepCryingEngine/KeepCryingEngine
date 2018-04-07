using KeepCryingEngine;

public class TranslateObjectOnUpdate : Component
{
    int i;

    public override void Update()
    {
        Debug.Log(i.ToString());
        i++;
    }
}