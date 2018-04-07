using KeepCryingEngine;

public class TranslateObjectOnUpdate : Component
{
    int i;

    public override void Update()
    {
        System.Console.WriteLine(i++);
    }
}