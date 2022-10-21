using System.IO;
using UnityEditor;
using UnityEngine;
using UnityEngine.SceneManagement;

public class GameObjectExportService : MonoBehaviour
{
    const string directoryPath = "Assets/_Exported";

    //　自前のメニューと項目を作成
    [MenuItem("MyTool/GameObject Export %t", false, 1)]
    static void GameObjectExport()
    {
        Scene scene = SceneManager.GetActiveScene();

        string filePath = directoryPath + "/" + scene.name + ".json";

        if (!Directory.Exists(directoryPath))
        {
            Directory.CreateDirectory(directoryPath);
        }
        using (StreamWriter writer = new StreamWriter(filePath))
        {
            writer.WriteLine("{");

            WriteTab(writer, 1);
            writer.WriteLine("\"objects\": [");

            //Scene scene = SceneManager.GetSceneByBuildIndex(0);

            // 取得したシーンのルートにあるオブジェクトを取得
            GameObject[] rootObjects = scene.GetRootGameObjects();

            // 取得したオブジェクトの名前を表示
            int count = 0;
            int rootObjectsCount = rootObjects.Length;
            foreach (GameObject obj in rootObjects)
            {
                WriteTab(writer, 2);
                writer.WriteLine("{");

                WriteGameObject(writer, obj.transform, 3);

                WriteTab(writer, 2);
                writer.WriteLine(count == rootObjectsCount - 1 ? "}" : "},");

                count++;
            }

            WriteTab(writer, 1);
            writer.WriteLine("]");

            writer.WriteLine("}");
        }
    }

    static void WriteGameObject(StreamWriter writer, Transform transform, int level)
    {
        WriteTab(writer, level);
        writer.WriteLine("\"name\" : \"{0}\",", transform.gameObject.name);

        string modelFilename = string.Empty;
        MeshFilter meshFilter = transform.GetComponent<MeshFilter>();
        if (meshFilter != null)
        {
            modelFilename = AssetDatabase.GetAssetPath(meshFilter.sharedMesh);
            CopyFile(modelFilename);
        }
        WriteTab(writer, level);
        writer.WriteLine("\"model_filename\" : \"{0}\",", modelFilename);

        string textureFilename = string.Empty;
        MeshRenderer meshRenderer = transform.GetComponent<MeshRenderer>();
        if (meshRenderer != null)
        {
            if (meshRenderer.sharedMaterial != null && meshRenderer.sharedMaterial.mainTexture)
            {
                Texture2D texture = (Texture2D)meshRenderer.sharedMaterial.mainTexture;
                textureFilename = AssetDatabase.GetAssetPath(texture);
                CopyFile(textureFilename);
            }
        }
        WriteTab(writer, level);
        writer.WriteLine("\"texture_filename\" : \"{0}\",", textureFilename);


        WriteTab(writer, level);
        writer.WriteLine("\"scale\" : {");
        WriteTab(writer, level + 1);
        writer.WriteLine("\"x\" : {0:G},", transform.localScale.x);
        WriteTab(writer, level + 1);
        writer.WriteLine("\"y\" : {0:G},", transform.localScale.y);
        WriteTab(writer, level + 1);
        writer.WriteLine("\"z\" : {0:G}", transform.localScale.z);
        WriteTab(writer, level);
        writer.WriteLine("},");

        WriteTab(writer, level);
        writer.WriteLine("\"rotate\" : {");
        WriteTab(writer, level + 1);
        writer.WriteLine("\"x\" : {0:G},", transform.localEulerAngles.x);
        WriteTab(writer, level + 1);
        writer.WriteLine("\"y\" : {0:G},", transform.localEulerAngles.y);
        WriteTab(writer, level + 1);
        writer.WriteLine("\"z\" : {0:G}", transform.localEulerAngles.z);
        WriteTab(writer, level);
        writer.WriteLine("},");

        WriteTab(writer, level);
        writer.WriteLine("\"translate\" : {");
        WriteTab(writer, level + 1);
        writer.WriteLine("\"x\" : {0:G},", transform.localPosition.x);
        WriteTab(writer, level + 1);
        writer.WriteLine("\"y\" : {0:G},", transform.localPosition.y);
        WriteTab(writer, level + 1);
        writer.WriteLine("\"z\" : {0:G}", transform.localPosition.z);
        WriteTab(writer, level);
        writer.WriteLine("},");

        WriteTab(writer, level);
        writer.WriteLine("\"objects\": [");

        for (int i = 0; i < transform.childCount; ++i)
        {
            Transform child = transform.GetChild(i);


            WriteTab(writer, level + 1);
            writer.WriteLine("{");

            WriteGameObject(writer, child, level + 2);

            WriteTab(writer, level + 1);
            writer.WriteLine(i == transform.childCount - 1 ? "}" : "},");
        }

        WriteTab(writer, level);
        writer.WriteLine("]");

        //if (transform.childCount == 0)
        //{
        //    WriteTab(writer, level);
        //    writer.WriteLine("\"objects\": []");
        //}
    }

    static void WriteTab(StreamWriter writer, int level)
    {
        for (int i = 0; i < level; ++i)
        {
            writer.Write("    ");
        }
    }

    static void CopyFile(string source)
    {
        string destination = Path.Combine(directoryPath, source.Remove(0, "Assets/".Length));
        if (!Directory.Exists(destination))
        {
            Directory.CreateDirectory(Path.GetDirectoryName(destination));
        }
        File.Copy(source, destination, true);
    }
}
