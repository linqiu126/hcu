<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hcuhchodatainfo */

$this->title = 'Create Hcuhchodatainfo';
$this->params['breadcrumbs'][] = ['label' => 'Hcuhchodatainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcuhchodatainfo-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
